require "digest/md5"
require "Eq"
require "Graph"

class InvalidMove < Exception
end

class Move
	include Eq

	attr_reader :player

	def initialize(player)
		super()
		@player = player
		@salt = ""
		10.times do
			@salt += (rand(?z-?a+1)+?a).chr
		end
	end

	def assert(reason, pred)
		if !pred
			raise InvalidMove.new(reason)
		end
	end

	def hash_repr
		Digest::MD5.hexdigest(repr)
	end
end

class PassMove < Move
	def initialize(player)
		super(player)
	end

	def adjucate(board)
		# always succeeds
	end

	def repr
		"P#@player:#@salt"
	end

	def to_s
		"Player #@player passes"
	end
end

class MotionMove < Move
	attr_reader :src, :dest
	
	def initialize(player, src, dest)
		super(player)
		@src, @dest = src, dest
	end
	
	def adjucate(board)
		assert("Player (#@player) must be alive to move",
			   board.alive?(@player))
		assert("Source (#@src) and destination (#@dest) must be connected",
			   board.graph.has_edge?(@src, @dest))
		assert("Source (#@src) must belong to player executing the move (#@player)", 
			   board.units[@src] == @player)
			   
		if board.supply[@dest] && board.supply[@dest] != @player
			atkstr = board.strength(@dest, @player)
			defstr = board.strength(@dest, board.supply[@dest]) + 1
			assert("Attacking strength (#{atkstr}) must be greater than defensive strength (#{defstr})",
				   atkstr > defstr)
			board.supply.delete(@dest)
		elsif board.units[@dest]
			assert("Cannot attack your own unit, stupid",
				   board.units[@dest] != @player)
			atkstr = board.strength(@dest, @player)
			defstr = board.strength(@dest, board.units[@dest])
			assert("Attacking strength (#{atkstr}) must be greater than defensive strength (#{defstr})",
				   atkstr > defstr)
		end

		board.units[@dest] = @player
		board.units.delete(@src)
	end

	def repr
		"M#@player:#@src:#@dest:#@salt"
	end
	
	def to_s
		"Player #@player moves from #@src to #@dest"
	end
end

class Board
	attr_accessor :units, :supply, :graph, :players, :turn, :view_center, :view_dim

	def initialize(graph, players)
		@graph = graph
		@units = {}
		@supply = {}
		@turn = 0
		@players = players
		@view_center = Vec.new(64,48)
		@view_dim = Vec.new(128,96)
	end

	def strength(pos, player)
		str = @units[pos] == player ? 1 : 0
		@units.each do |v,p|
			if @graph.has_edge?(pos, v) && @units[v] == player
				str += 1
			end
		end
		str
	end

	def alive?(player)
		units = 0
		@units.each do |v,p|
			units += 1 if p == player
		end

		tcenters = 0
		centers = 0
		@supply.each do |v,p|
			tcenters += 1
			centers += 1 if p == player
		end

		units > 1 && (tcenters == 0 || centers > 0)
	end

	def next_turn
		@turn += 1
		@turn %= @players
	end

	def copy
		c = clone
		c.units = c.units.clone
		c.supply = c.supply.clone
		c
	end

	# returns a new board with the given moves incorporated
	def adjucate(moves)
		board = copy
		moves.each do |m| 
			m.adjucate(board)
			yield(m, board) if block_given?
		end
		board
	end

	# returna a new board with the given moves incoprporated,
	# nil (instead of exception) on failure
	def try_adjucate(moves, &code)
		begin
			adjucate(moves, &code)
		rescue InvalidMove => x
			nil
		end
	end

	def Board::select_level(create = nil)
		raise "Argument must be nil or :create" unless create == nil || create == :create
		
		levels = []
		Dir.new('levels').each do |level|
			unless /^\./ === level
				levels << level
			end
		end

		levels.sort!
		puts "Levels:"
		levels.each_with_index do |level, idx|
			puts "  #{idx+1}: #{level}"
		end

		levelno = nil
		1.times do
			if create == :create
				print "Level number (or name for new)? "
			else
				print "Level number? "
			end
			levelno = gets.chomp
			unless /^-?\d+$/ === levelno
				if create == :create
					return "levels/#{levelno}"
				else
					puts "Uh, give me a number."
					redo
				end
			end
			
			unless 0 < levelno.to_i && levelno.to_i <= levels.length
				puts "That is a number, but it's a stupid answer."
				redo
			end

			return "levels/#{levels[levelno.to_i-1]}"
		end
	end
end
