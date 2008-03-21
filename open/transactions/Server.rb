require "socket"
require "gserver"
require "Player"
require "Artist"
require "Vec"

class IPListener < GServer
	def initialize(port = 4201)
		super(port, "64.22.71.204", 64)  # 64 max connections
		@games = {}
	end

	def start
		log "Listening for connections on port #@port"
		super
	end

	def serve(io)
		begin
			@games.delete_if do |k,v| 
                unless v.alive?
                    log "Game #{k} pruned"
                    true
                else
                    false
                end
            end
			
			while true
				request = Marshal.load(io)
				case request['action']
				when 'NEWGAME'
					game = Server.new(request['board'], request['time'])
					@games[request['name']] = game
					log "New game '#{request['name']}' created"
					game.join(io)
					return
				when 'JOIN'
					log "Game '#{request['name']}' joined"
					@games[request['name']].join(io)
					return
				when 'LIST'
					log "Gamelist sent"
					Marshal.dump({
						'action' => 'GAMELIST',
						'games' => @games.keys,
					}, io)
				end
			end
		rescue => x
			log x.message
			x.backtrace.each do |line|
				log "    " + line
			end
		end
	end
end

class Server
	def initialize(board, time)
		@time = time
		@board = board
		@players = []
		@join_mutex = Mutex.new
		@turn = @board.turn
		@names = []
		@done = false
	end

	def alive?
		!@done
	end

	def validate_turn
		@turn %= @board.players
		initurn = @turn
		until @turn < @players.length && @players[@turn].alive?
			@turn += 1
			@turn %= @board.players
			break if @turn == initurn
		end
		@players[@turn].send({ 'action' => 'YOUR_TURN' })
	end

	def join(io)
		@join_mutex.synchronize do
			if @players.length < @board.players
				session = Connection.new(io, @players.length)
				@players << session
				Thread.current['playerno'] = session.playerno
				session.send({ 'action' => 'INIT', 'player' => session.playerno, 'board' => @board, 'time' => @time })

				ident = session.recv
				if ident['action'] == 'IDENT'
					@names << ident['name']
					puts "#{@players.length} players connected"
				else
					puts "Bad connection."
				end

				nmhash = {}
				@names.each_with_index do |n,i|
					nmhash[i] = { 'name' => n, 'alive' => @players[i].alive? }
				end
				
				broadcast({
					'action' => 'NAMES',
					'names' => nmhash,
				})

				if @players.length == @board.players
					broadcast({
						'action' => 'START',
					})

					validate_turn
				end
			elsif @players.length >= @board.players
				io.close
				return
			end
		end
		run
	end

	def broadcast(data)
		@players.each { |p| p.send(data) }
	end
		
	def run
		while true
			if !@done && @players.select { |p| p.alive? }.length == 0
                puts "Game dead"
				@done = true
			end
			if @done
				return
			end

			conn = @players[Thread.current['playerno']]
			if conn.alive?
				response = conn.recv
				puts "Received #{response['action']} response from player #{Thread.current['playerno']}"
				broadcast(response)

				if response['action'] == 'DONE'
					@turn += 1
					validate_turn
				elsif response['action'] == 'DISCONNECT'
					validate_turn
				end
			else
				return
			end
		end
	end
end

class Connection
	attr_accessor :playerno
	
	def initialize(session, playerno)
		@playerno = playerno
		@session = session
		@disconnect = false
	end

	def send(msg)
		if @session
			begin
				Marshal.dump(msg, @session)
				return
			rescue => ex
				puts "Player #@playerno lost connection: #{ex.message}"
				@session = nil
				@disconnect = true
			end
		end
	end

	def recv
		if @session
			begin
				return Marshal.load(@session)
			rescue => ex
				puts "Player #@playerno lost connection #{ex.message}"
				@disconnect = true
				@session = nil
			end
		end

		if @disconnect
			@disconnect = false
			{ 'action' => 'DISCONNECT', 'player' => @playerno }
		else
			raise "Read from dead connection"
		end
	end

	def alive?
		@session != nil
	end
end

class Browser
	def initialize(name, server = 'localhost', port = 4201)
		@name = name
		if /^http:\/\/(.*)/ === server
			@session = TCPSocket.new($1, 80)
			@session.print "CONNECT localhost:#{port} HTTP/1.0\n\n"
			until @session.gets.chomp.to_s == ""; end
		else
			@session = TCPSocket.new(server, port)
		end
	end

	def ask(question, default = nil)
		print question
		ans = gets.chomp
		ans == "" ? default : ans
	end

	def run
		Marshal.dump({
			'action' => 'LIST',
		}, @session)

		games = Marshal.load(@session)['games']

		if games.length > 0
			puts "Games:"
			games.each_with_index do |game, idx|
				puts "  #{idx+1}: #{game}"
			end

			selection = ask("Which game (blank for new)? ")
		else
			puts "No games.  Creating a new one."
			selection = nil
		end

		if selection
			gameno = selection.to_i - 1
			Marshal.dump({
				'action' => 'JOIN',
				'name' => games[gameno],
			}, @session)
		else
			name = ask("Game Name? ", @name + "'s game")

			board = open(Board.select_level, "r") { |io| Marshal.load(io) }
			
			time = 60 * ask("Time per player (in minutes)? ", 1.0/0.0).to_f

			Marshal.dump({ 
				'action' => 'NEWGAME', 
				'name' => name,
				'board' => board,
				'time'  => time,
			}, @session)
		end

		Client.new(@name, @session).run
	end
end

class Client
	def initialize(name, session)
		@session = session
		@time = nil
		data = Marshal.load(@session)
		raise "First action should be INIT" unless data['action'] == "INIT"
		send({
			'action' => 'IDENT',
			'name'   => name,
		})
		@player = GUIPlayer.new(data['player'], data['board'], data['time'], self)
	end

	def run
		begin
			while true
				action = Marshal.load(@session)
				case action['action']
				when 'START'
					@player.start
				when 'NAMES'
					@player.set_player_names(action['names'])
				when 'YOUR_TURN'
					@time = Time.now
					@player.your_turn
				when 'MOVE'
					@player.record_move(action['player'], action['digest'])
				when 'SCRATCH'
					@player.record_scratch(action['player'])
				when 'COMMIT'
					@player.record_commit(action['player'], action['moves'])
				when 'DONE'
					@player.record_done(action['player'], action['time'])
				when 'DISCONNECT'
					@player.record_disconnect(action['player'])
				when 'CHAT'
					@player.record_chat(action['player'], action['text'])
				else
					raise "I don't know what #{action['action']} means"
				end
			end
		rescue ExitPlayer
		end
	end

	def send(action)
		Marshal.dump(action, @session)
	end
	
	def move(mv)
		send({
			'action' => 'MOVE',
			'player' => @player.playerno,
			'digest' => mv.hash_repr,
		})
	end

	def commit(moves)
		send({
			'action' => 'COMMIT',
			'player' => @player.playerno,
			'moves'  => moves,
		})
	end

	def scratch
		send({
			'action' => 'SCRATCH',
			'player' => @player.playerno,
		})
	end

	def done
		send({
			'action' => 'DONE',
			'player' => @player.playerno,
			'time'    => Time.now - @time,
		})
		@time = nil
	end

	def chat(text)
		send({
			'action' => 'CHAT',
			'player' => @player.playerno,
			'text'   => text,
		})
	end
end
