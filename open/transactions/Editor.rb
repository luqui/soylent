require "Board"
require "Artist"
require "Vec"
require "Util"

class Editor
	def initialize(file)
		@board = Board.new(Graph.new, 0)
		@board.graph.artist = GraphArtist.new(@board.graph)
		@selected = nil
		@fullscreen = false
		@file = file
		@view_center = Vec.new(Util::SCREEN_WIDTH/2, Util::SCREEN_HEIGHT/2)
		@view_dim    = Vec.new(Util::SCREEN_WIDTH, Util::SCREEN_HEIGHT)

		if File.exist?(@file)
			load_board(@file)
		end
	end

	def run
		GLUT.InitWindowSize 640, 480
		GLUT.InitDisplayMode GLUT::DOUBLE | GLUT::RGB
		GLUT.Init
		GLUT.CreateWindow
		GLUT.DisplayFunc(proc { display })
		GLUT.MouseFunc(proc { |*xs| mouse(*xs) })
		GLUT.KeyboardFunc(proc { |*xs| keyboard(*xs) })
		GLUT.SpecialFunc(proc { |*xs| special(*xs) })
		
		GL.MatrixMode GL::PROJECTION
		GL.LoadIdentity
		GLU.Ortho2D 0, 128, 0, 96
		GL.MatrixMode GL::MODELVIEW

		GLUT.MainLoop
	end

	def display
		GL.Clear GL::COLOR_BUFFER_BIT
		GL.LoadIdentity
		draw
		GLUT.SwapBuffers
	end

	def draw
		Util.layout(@view_center - @view_dim/2, @view_center + @view_dim/2) do
			BoardArtist.new(@board).draw_graph
			BoardArtist.new(@board).draw
			
			if @selected
				pos = @board.graph.artist.get_pos(@selected)
				GL.PushMatrix
				GL.Translate pos.x, pos.y, 0
				GL.Scale 1.5, 1.5, 0
				GL.LineWidth 2
				GL.Color 0,1,0
				Util.draw_circle
				GL.LineWidth 1
				GL.PopMatrix
			end
		end
	end

	def mouse(button, state, x, y)
		pos = Vec.new(x * @view_dim.x / GLUT.Get(GLUT::WINDOW_WIDTH) + @view_center.x - @view_dim.x/2, 
					  (GLUT.Get(GLUT::WINDOW_HEIGHT)-y) * @view_dim.y / 
					    GLUT.Get(GLUT::WINDOW_HEIGHT) + @view_center.y - @view_dim.y/2)
		if button == GLUT::LEFT_BUTTON && state == GLUT::DOWN
			vertex = @board.graph.artist.pick_vertex(pos)
			if vertex == nil
				vertex = @board.graph.add_vertex
				@board.graph.artist.set_pos(vertex, pos)
			end

			if @selected
				@board.graph.add_edge(@selected, vertex)
			end
			@selected = vertex
		elsif button == GLUT::RIGHT_BUTTON && state == GLUT::DOWN
			vertex = @board.graph.artist.pick_vertex(pos)
			if vertex
				if @board.supply[vertex]
					@board.supply.delete(vertex)
				elsif @board.units[vertex]
					@board.units.delete(vertex)
				else
					@board.graph.delete_vertex(vertex)
				end
			else
				edge = @board.graph.artist.pick_edge(pos)
				if edge
					@board.graph.delete_edge(*edge)
				end
			end
			@board.graph.artist.prune
			@selected = nil
		end
		update
	end

	MODMAP = {
		?! => ?1,
		?@ => ?2,
		?# => ?3,
		?$ => ?4,
		?% => ?5,
		?^ => ?6,
		?& => ?7,
		?* => ?8,
		?( => ?9,
	}

	def keyboard(key, x, y)
		if key >= ?1 && key <= ?9 && @selected
			@board.supply[@selected] = key - ?1
		elsif MODMAP[key] && MODMAP[key] >= ?1 && MODMAP[key] <= ?9 && @selected
			@board.units[@selected] = MODMAP[key] - ?1
		elsif key == ?s
			save_board(@file)
		elsif key == ?l
			load_board(@file)
		elsif key == ?\e
			save_board(@file)
			exit
		elsif key == ?f
			if @fullscreen
				GLUT.ReshapeWindow(640, 480)
				@fullscreen = false
			else
				GLUT.FullScreen
				@fullscreen = true
			end
		elsif key == ?*
			@view_dim /= 1.1
		elsif key == ?/
			@view_dim *= 1.1
		end
		update
	end

	def special(sym, x, y)
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

	def update
		GLUT.PostRedisplay
	end

	def compute_players
		pls = Set.new
		@board.units.each do |pos,player|
			pls.add(player)
		end
		@board.players = pls.length
	end

	def save_board(file)
		compute_players
		@board.view_center = @view_center
		@board.view_dim = @view_dim
		open(file, "w") do |io|
			Marshal.dump(@board, io)
		end
	end

	def load_board(file)
		if File.exist?(file)
			open(file, "r") do |io|
				@board = Marshal.load(io)
			end
		end

		@view_center = @board.view_center if @board.view_center
		@view_dim = @board.view_dim if @board.view_dim
	end
end
