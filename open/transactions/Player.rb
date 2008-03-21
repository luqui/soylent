require "Board"
require "opengl"
require "glut"
require "thread"
require "monitor"
require "Coroutine"
require "Util"
require "GlutSucksTimer"

class ExitPlayer < Exception
end

class CheatingException < Exception
end

class Opponent
	attr_accessor :name, :disconnected, :time
	
	def initialize
		@name = "-"
		@disconnected = false
		@transaction = []
	end

	def scratch
		@transaction = []
	end

	def move(digest)
		@transaction << digest
	end

	def pending
		@transaction.length
	end

	def commit(moves)
		unless moves.length == @transaction.length
			raise CheatingException.new("Number of moves does not match (sent: #{moves.length}; recorded #{@transaction.length}")
		end

		@transaction.zip(moves).each do |h,m|
			unless m.hash_repr == h
				raise CheatingException.new("Move hash for #{m} does not match recorded hash")
			end
		end

		@transaction = []
	end
end

class Player
	include MonitorMixin
	attr_reader :playerno
	
	def initialize(playerno, board, time, server)
		super()

		@board = board
		@cur_moved = false
		@playerno = playerno

		@opponents = []
		@board.players.times do |p|
			@opponents[p] = Opponent.new
			@opponents[p].time = time
		end
		
		@transaction = []
	end

	def update
	end

	def start
		synchronize {
		validate_turn
		}
	end
	
	def set_player_names(names)
		synchronize {
		names.each do |p,n|
			@opponents[p].name = n['name']
			@opponents[p].disconnected = !n['alive']
		end
		update
		}
	end
	
	def record_move(player, digest)
		synchronize {
		unless @board.turn == player && !@cur_moved
			raise CheatingException.new("Opponent moved out of turn")
		end
		puts "Player #{player} moved: #{digest}"
		@opponents[player].move(digest)
		@cur_moved = true
		}
	end

	def record_scratch(player)
		synchronize {
		puts "Player #{player} scratched his transaction"
		@opponents[player].scratch
		}
	end
	
	def scratch
		synchronize {
		@transaction = []
		}
	end

	def record_commit(player, moves)
		synchronize {
		@opponents[player].commit(moves)

		if player == @playerno
			@transaction = []
		end
		
		puts "Player #{player} committed his transaction:"
		begin
			@board = @board.adjucate(moves) do |move,board|
				puts "    #{move}"
				scratch unless board.try_adjucate(@transaction)
			end
		rescue InvalidMove => x
			raise CheatingException.new("Opponent invalid move: " + x.message)
		end
		
		update
		}
	end

	def validate_turn
		synchronize {
		pturn = @board.turn
		while @opponents[@board.turn].disconnected
			@board.next_turn
			break if @board.turn == pturn
		end
		}
	end
	
	def record_done(player, time)
		synchronize {
		@board.next_turn
		validate_turn
		@cur_moved = false
		@opponents[player].time -= time
		update
		}
	end

	def record_disconnect(player)
		synchronize {
		@opponents[player].disconnected = true
		validate_turn
		}
	end

	def transaction_board(extra = [])
		synchronize {
		@board.adjucate(@transaction + extra)
		}
	end

end

class GUIPlayer < Player
	def initialize(playerno, board, time, server)
		super

		@server = server
		@local_transaction = [[], nil]
		@myturn = false
		@fullscreen = false
		@queue = []
		@action_queue = []
		@chatbox = []
		@chat_text = nil
		@turntime = nil
		@showhelp = false
		@showshowhelp = true
		@timer = GlutSucksTimer.new
		@view_center = board.view_center || Vec.new(64,48)
		@view_dim    = board.view_dim    || Vec.new(128,96)

		@thread = Thread.current
		Thread.new do
			make_mouse_routine
			Thread.current.abort_on_exception = true
			
			GLUT.InitWindowSize 640, 480
			GLUT.InitDisplayMode GLUT::DOUBLE | GLUT::RGB
			GLUT.Init
			GLUT.CreateWindow
			GLUT.DisplayFunc(proc { display })
			GLUT.MouseFunc(proc { |*xs| mouse(*xs) })
			GLUT.KeyboardFunc(proc { |*xs| keyboard(*xs) })
			GLUT.SpecialFunc(proc { |*xs| special(*xs) })
			GLUT.IdleFunc(proc { sleep 0.05; Thread.pass })

			GL.Enable(GL::BLEND)
			GL.Enable(GL::LINE_STIPPLE)
			GL.BlendFunc(GL::SRC_ALPHA, GL::ONE_MINUS_SRC_ALPHA)
			GL.MatrixMode GL::PROJECTION
			GL.LoadIdentity
			GLU.Ortho2D 0, Util::SCREEN_WIDTH, 0, Util::SCREEN_HEIGHT
			GL.MatrixMode GL::MODELVIEW

			delay = nil
			delay = proc do
				update
				@timer.delay 1.0, &delay
			end
			@timer.delay 1.0, &delay
			
			GLUT.MainLoop
		end
	end	

	def update
		synchronize {
		GLUT.PostRedisplay
		}
	end

	def make_mouse_routine
		@selected_vertices = []
		@mouse = Coroutine.new do |va|
			while true
				@selected_vertices = [ [ va, [ 0, 1, 0 ] ] ]  # va, colored green
				vb = @mouse.yield
			
				make_move(MotionMove.new(@playerno, va, vb))
				va = vb
			end
		end
	end

	def mouse(button, state, x, y)
		synchronize {
		pos = Vec.new(x * @view_dim.x / GLUT.Get(GLUT::WINDOW_WIDTH) + @view_center.x - @view_dim.x/2, 
		              (GLUT.Get(GLUT::WINDOW_HEIGHT)-y) * @view_dim.y / 
					     GLUT.Get(GLUT::WINDOW_HEIGHT) + @view_center.y - @view_dim.y/2)
		
		if button == GLUT::LEFT_BUTTON && state == GLUT::DOWN
			vertex = @board.graph.artist.pick_vertex(pos)
			if vertex
				@mouse.call(vertex)
			else
				make_mouse_routine
			end
		end
		
		if button == GLUT::RIGHT_BUTTON && state == GLUT::DOWN
			make_mouse_routine
		end

		update
		}
	end

	def keyboard(key, x, y)  # wtf are x and y?
		synchronize {

		if @chat_text
			case key
			when ?\r
				text = @chat_text
				action do
					@server.chat(text)
				end
				@chat_text = nil
			when ?\b
				@chat_text = @chat_text[0..-2]
			when ?\e
				@chat_text = nil
			else
				@chat_text += key.chr
			end
			update
			return
		end

		case key
		when ?x
			action do
				@queue = []
				@action_queue = []
			end
		when ?X
			action do
				scratch
			end
		when ' '[0]
			act = proc do
				send_move
				done
			end
			if @myturn
				action &act
			else 
				if @action_queue.length < @queue.length
					@action_queue.push([:enqueue, act])
				end
			end
		when ?\r
			act = proc do
				send_move
				@server.commit(@transaction)
				done
			end
			if @myturn
				action &act
			else
				if @action_queue.length < @queue.length
					@action_queue.push([:commit, act])
				end
			end
		when ?u
			@queue.pop
			# *This* is the reason that you don't want run-time dwimmery
			if @queue.length > 0
				@action_queue = @action_queue[0..@queue.length-1]
			else
				@action_queue = []
			end
		when ?\e
			@thread.raise(ExitPlayer.new)
		when ?\t
			@chat_text = ""
		when ?f
			if @fullscreen
				@fullscreen = false
				GLUT.ReshapeWindow(640, 480)
			else
				@fullscreen = true
				GLUT.FullScreen
			end
		when ?h
			@showshowhelp = false
			@showhelp = !@showhelp
		when ?/
			@view_dim *= 1.1
			update
		when ?*
			@view_dim /= 1.1
			update
		end	

		update
		}
	end

	def special(sym, x, y)  # see wtf above
		case sym
		when GLUT::KEY_LEFT
			@view_center.x -= 0.1 * @view_dim.x
			update
		when GLUT::KEY_RIGHT
			@view_center.x += 0.1 * @view_dim.x
			update
		when GLUT::KEY_DOWN
			@view_center.y -= 0.1 * @view_dim.y
			update
		when GLUT::KEY_UP
			@view_center.y += 0.1 * @view_dim.y
			update
		end
	end

	def display
		synchronize {
		GL.Clear GL::COLOR_BUFFER_BIT
		GL.LoadIdentity
		draw
		GLUT.SwapBuffers
		}
	end

	def send_move
		synchronize {
		if @queue.length > 0
			move = @queue.shift
			@transaction.push(move)
			@server.move(move)
		else
			pass = PassMove.new(@playerno)
			@transaction.push(pass)
			@server.move(pass)
		end
		}
	end

	def make_move(move)
		synchronize {
		tboard = transaction_board(@queue)
		
		begin
			move.adjucate(tboard)
		rescue InvalidMove => x
			puts "That is an invalid move: #{x.message}"
		else
			@queue.push(move)
			update
		end
		}
	end

	def start
		synchronize {
		super
		@turntime = Time.now
		}
	end
	
	def scratch
		synchronize {
		super
        @action_queue = []
		@queue = []
		@server.scratch
		}
	end
	
	def record_commit(player,moves)
		synchronize {
		if moves.length != 0 && player != @playerno
			@local_transaction = [moves, @board.copy]
		end

		super

		unless @board.try_adjucate(@transaction + @queue)
			@queue = []
			@action_queue = []
		end
		}
	end

	def record_chat(player, text)
		synchronize {
		@chatbox.unshift([player, @opponents[player].name + "> " + text])
		@timer.delay 9.0 do
			synchronize {
				@chatbox.pop
				update
			}
		end
		update
		}
	end
	
	def record_done(player, time)
		synchronize {
		super
		@turntime = Time.now
		}
	end

	def scale_alpha(color, amt)
		synchronize {
		newcol = color.clone
		if newcol[3]
			newcol[3] *= amt
		else
			newcol[3] = amt
		end
		newcol
		}
	end
	
	def draw
		synchronize {
		Util.layout(@view_center - @view_dim/2, @view_center + @view_dim/2) do
			BoardArtist.new(@board).draw_graph
			BoardArtist.new(transaction_board(@queue)).draw
			
			realscheme = DEFAULT_SCHEME.clone
			realscheme.length.times do |i|
				realscheme[i] = scale_alpha(realscheme[i], 0.45)
			end
			
			BoardArtist.new(@board).draw(realscheme)

			@selected_vertices.each do |v|
				pos = @board.graph.artist.get_pos(v[0])
				GL.PushMatrix
				GL.Translate pos.x, pos.y, 0
				GL.Scale 1.5, 1.5, 0
				GL.LineWidth 2
				GL.Color *v[1]
				Util.draw_circle
				GL.LineWidth 1
				GL.PopMatrix
			end

			trace_transaction(@local_transaction[0], @local_transaction[1])
			trace_transaction(@transaction, @board)
			GL.LineStipple(8, 0b1010101010101010)
			trace_transaction(@queue, @board)
			GL.LineStipple(1, 0b1111111111111111)
		end

		# HUD stuff
		Util.layout(Vec.new(0,0), Vec.new(Util::SCREEN_WIDTH, Util::SCREEN_HEIGHT)) do 
			if @myturn
				GL.Color *DEFAULT_SCHEME[@playerno]
				GL.RasterPos 1, 1
				Util.draw_string("Your turn")
			end

			xpos = Util::SCREEN_WIDTH
			(@board.players-1).downto(0) do |p|
				if !alive?(p)
					string = "#{@opponents[p].name} (DEAD)"
				elsif @opponents[p].disconnected
					string = "#{@opponents[p].name} (GONE)"
				else
					time = @opponents[p].time
					if time.finite?
						if @turntime && @board.turn == p
							time -= (Time.now - @turntime)
						end
						time = 0 if time < 0
						minutes = (time / 60).to_i
						seconds = (time % 60).to_i
						timestr = sprintf("%d:%02d ", minutes, seconds)
					else
						timestr = ""
					end
					pending = p == @playerno ? @transaction.length : @opponents[p].pending
					string = "#{@opponents[p].name} #{timestr}(#{pending})"
				end

				xpos -= (string.length + 3) * Util.scr_char_width

				GL.Color *DEFAULT_SCHEME[p]
				GL.RasterPos xpos, 1

				Util.draw_string(string)

				# draw a box around the active player
				if @board.turn == p
					GL.PushMatrix
					GL.Translate xpos, 1, 0
					GL.Begin GL::LINE_LOOP
					Util.box_vertices Vec.new(-1,-1), Vec.new(string.length * Util.scr_char_width, Util.scr_char_height)
					GL.End
					GL.PopMatrix
				end
			end

			ypos = Util::SCREEN_HEIGHT
			xpos = Util::SCREEN_WIDTH - 4
			@action_queue.each do |type,code|
				ypos -= 2
				case type
				when :enqueue then GL.Color(0.6, 0.6, 0.6)
				when :commit  then GL.Color(1,1,1)
				else raise "I don't know how to draw #{type}"
				end

				GL.Begin GL::QUADS
				Util.box_vertices(Vec.new(xpos, ypos), Vec.new(xpos+3, ypos+1))
				GL.End
			end

			Util.draw_text_box :left, 2, :bottom, 5 * Util.scr_char_height - 1,
				@chatbox.reverse.map { |p,t| [DEFAULT_SCHEME[p], t] }

			if @chat_text
				GL.Color 1,1,1
				GL.RasterPos 3, 3 * Util.scr_char_height
				Util.draw_string("> " + @chat_text)
			end

			if @showshowhelp
				GL.Color 1,1,1
				text = "press 'h' for help"
				GL.RasterPos Util::SCREEN_WIDTH - text.length * Util.scr_char_width - 1, 2.5 * Util.scr_char_height
				Util.draw_string(text)
			end

			if @showhelp
				Util.draw_text_box :right, 2, :bottom, 5 * Util.scr_char_height - 1,
					["space  - end turn",
					 "enter  - end turn and commit",
					 "u      - undo",
					 "x      - clear queue",
					 "X      - cancel transaction",
					 "tab    - chat",
					 "f      - full screen",
					 "*,/    - zoom",
					 "arrows - scroll",
					 "escape - quit",
					 "h      - help"].map { |t| [[1,1,1],t] }
			end
		end
		}
	end

	def trace_transaction(trans, refboard)
		synchronize {
		GL.LineWidth 3
		trans.each do |move|
			if move.is_a?(MotionMove)
				if refboard.units[move.dest]  && refboard.units[move.dest]  != move.player ||
				   refboard.supply[move.dest] && refboard.supply[move.dest] != move.player
					pos = @board.graph.artist.get_pos(move.dest)
					GL.Color 1,0,0
					GL.PushMatrix
					GL.Translate pos.x, pos.y, 0
					GL.Scale 1.6, 1.6, 1
					Util.draw_circle
					GL.PopMatrix
				end
			
				GL.Color *scale_alpha(DEFAULT_SCHEME[move.player], 0.65)
				@board.graph.artist.draw_edge(move.src, move.dest)
			end
		end
		GL.LineWidth 1
		}
	end

	def action
		synchronize {
		yield
		update
		}
	end

	def done
		synchronize {
		@server.done
		@myturn = false
		}
	end

	def your_turn
		synchronize {
		if alive?(@playerno)
			if @action_queue.length > 0
				@action_queue.shift()[1].call
			else
				@myturn = true
			end
		else
			scratch
			@server.done
			@myturn = false
		end
		update
		}
	end
	
	def alive?(player)
		synchronize {
		@board.alive?(player) && @opponents[player].time > 0
		}
	end
end
