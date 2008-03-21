# = AI - Base class for AI controllers

class AI
	# AIs that have been directly assigned to a unit have their 'step' function
	# called once per frame.  If an AI has been assigned to more than one unit,
	# you should shoot me for doing that (step will be called once for each
	# unit each frame).
	
	def step; end

	# AIs assigned to a unit have disconnect called on them when
	#   1) the unit is destroyed
	#   2) a different AI is assigned to that unit
	
	def disconnect; end

	# AI::Success and AI::Failure are two classes that are used to represent
	# the success or failure of an order. 
	
	class Success
		# .ok does nothing on success and raises an exception on failure
		# (it raises the failure object as the exception)
		def ok; end

		# .err does nothing on success and calls the given block on failure
		#
		#   child.order(PathFinder::MoveTo.new(dest)).err do
		#   	env.respond(TacticFailed.new)
		#   end

		def err; end
	end

	# See AI::Success for a description of these methods.
	
	class Failure < Exception
		def ok
			raise self
		end

		def err
			yield self
		end
	end

	# Order a unit to follow a gradient
	#   ai.order(env(DiffusionFollower::Follow.new(grad, proc { |p| done? })))
	
	FollowGradient = Struct.new("Follow", :grad, :pred)
	
	# Order a unit to move to a location
	class MoveTo
		attr_reader :dest

		def initialize(dest)
			@dest = dest
		end
	end

	# Notification that the unit has arrived successfully.
	class Arrived   < AI::Success; end

	# Notification that the AI has been given another order
	# while one was in progress.
	class Cancelled < AI::Failure; end
	
	# Notification that the unit was obstructed in its course.
	class Obstructed < AI::Failure; end
end
