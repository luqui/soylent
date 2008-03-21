# = LinearMover - AI controller that moves units in straight lines
#
# This is now an unused AI, kept here probably to serve as some sort of example.
# It may just be killed in the future, because PathFinder is a more practical
# simple AI.

require "AI/AI"
require "Vec"

class LinearMover < AI
	attr_reader :unit, :dest
	
	def initialize(unit)
		super
		@unit = unit
		@dest = @unit.get_position
		@moving = false
		@env = nil
		@decel = @unit.get_speed_aptitude.get_deceleration
		unit.set_ai(self)
	end

	def order(env)
		case env.msg
		when MoveTo 
			# notify our commander that the command was cancelled by another order
			@env.respond(Cancelled.new) do  # <-- means do the block, then respond
				@env = env
				@dest = @env.msg.dest
				@moving = true
				set_path
				env.ack
			end
		else raise "LinearMover cannot respond to #{env.msg}"
		end
	end

	# This AI stops its unit when disconnected.
	
	def disconnect
		@moving = false
		@unit.set_velocity(Vec.new(0,0))
		@env.respond(Cancelled.new) { @env = nil }
	end

	# If there is an active order, the unit is moved in a straight line
	# toward its destination until it arrives (the unit slows down toward
	# its arrival site to give time for deceleration) or until it is 
	# obstructed.
	
	def step
		if @moving
			pos = @unit.get_position
			stopdist = 0.5 * @unit.get_velocity.norm2 / @decel
			if (pos - @dest).norm <= stopdist
				@moving = false
				@unit.set_velocity(Vec.new(0,0))
				@env.respond(Arrived.new)	
			elsif @unit.obstructed
				@moving = false
				@unit.set_velocity(Vec.new(0,0))
				@env.respond(Obstructed.new)
			end
		end
	end

  protected

	def set_path
		pos = @unit.get_position
		@unit.set_velocity(~(@dest - pos) * @unit.get_speed_aptitude.get_max_speed)
	end
end
