# = PathFinder - tell a unit where to go

require "AI/DiffusionFollower"

class PathFinder < AI
	include Stream
	
	def initialize(unit)
		super()
		@child = DiffusionFollower.new(unit)
	end

	def order(env)
		case env.msg
		when MoveTo
			@env.respond(Cancelled.new) do
				pathfind = PathFind.new
				pathfind.set_dest(env.msg.dest)
				pathfind.set_speed_apt(@child.unit.get_speed_aptitude)
				@env = env
				message = FollowGradient.new(pathfind,
											 proc { |p| p.quantize == env.msg.dest.quantize })
				async(@child, message) { |r| @env.respond(r) }
				@env.ack
			end
		else raise "PathFinder cannot respond to #{env.msg}"
		end
	end
end
