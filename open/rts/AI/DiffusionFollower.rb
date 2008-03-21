# = DiffusionFollower - direct a unit based on a diffusion field

require "AI/AI"
require "Vec"

# DiffusionFollower receives messages that give a unit a diffusion field
# to follow and a predicate for when it has finished.  The unit simply
# carries out its task.  It is a layer below the PathFinder AI.

class DiffusionFollower < AI
	attr_reader :unit
	
	def initialize(unit)
		super()
		@unit = unit
		@env = nil
		@grad = nil
		@pred = nil
		@unit.set_ai(self)
	end


	def order(env)
		case env.msg
		when FollowGradient
			@env.respond(Cancelled.new) do
				@env  = env
				@grad = @env.msg.grad
				@pred = @env.msg.pred
				@env.ack
			end
		else raise "DiffusionFollower cannot respond to #{env.msg}"
		end
	end

	# When this AI is disconnected, it stops its unit.
	def disconnect
		@env.respond(Cancelled.new) { reset }
	end

	# Computes the local gradient and moves in the primary direction
	# of steepest descent; i.e. up, down, left, right, and the diagonals.
	# If a proper gradient (computed as the moment of these eight positions)
	# is used, the movement is jittery - the unit ends up turning small 
	# amounts too much.  It would be nice to find a happy medium.
	#
	# In the case where the unit is sitting on a local minimum but is not at
	# its destination (which cannot happen through the PathFind algorithms
	# themselves - the gradient would have to be augmented) then the unit will
	# go in the shallowest ascending direction, probably resulting in
	# oscillatory behavior.  This is a problem.
	
	def step
		if @grad
			pos = @unit.get_position
			# To minimize jerky movement, we find the minimal primary direction,
			# instead of the proper gradient
			dir = Vec.new(0,0)
			minwt = 1.0/0.0   # IEEE says this is +infinity
			for i in -1..1
				for j in -1..1
					next if i == 0 && j == 0
					val = @grad.get_weight(pos + Vec.new(i,j))
					if val < minwt
						dir = Vec.new(i,j)
						minwt = val
					end
				end
			end

			if @pred.call(pos)
				@env.respond(Arrived.new) { reset }
			else
				apt = @unit.get_speed_aptitude
				speed = apt.get_max_speed
				@unit.set_velocity(~dir * speed)
			end
		end
	end

  protected
  
	def reset
		@unit.set_velocity(Vec.new(0,0))
		@env, @grad, @pred = nil
	end
end
