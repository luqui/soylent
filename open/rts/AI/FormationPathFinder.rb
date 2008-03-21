# = FormationPathFinder - tell several units where to go

require "AI/Stream"
require "AI/DiffusionFollower"
require "Vec"

class FormationPathFinder < AI
	include Stream
	
	def initialize(units)
		struct = Struct.new(:unit, :ai, :offset, :lead)
		
		center = units[0].get_position
		@units = units.map do |unit|
			struct.new(unit, DiffusionFollower.new(unit), unit.get_position - center, false)
		end

		@units[0].lead = true

		@env = nil
		@dest = nil
		@moving = nil
		@success = false
	end

	def order(env)
		case env.msg
		when MoveTo
			@env.respond(Cancelled.new) do
				@env = env
				@dest = @env.msg.dest
				@moving = @units.length
				@success = false
				pf = PathFind.new
				pf.set_dest(@dest)
				pf.set_speed_apt(@units[0].unit.get_speed_aptitude) # XXX minimum, not first
				
				@units.each do |unit|
					done = proc do |p|
						(p - (@dest + unit.offset)).norm < 1
					end

					grad = unit.lead ? pf : FormationGradient.new(pf, @units[0], @dest, unit.offset, unit.unit.get_speed_aptitude.get_max_speed)
					async(unit.ai, FollowGradient.new(grad, done)) do |r|
						# check return code
						s = true
						r.err { s = false }
						@success ||= s
						@moving -= 1

						if @moving == 0
							if @success
								@env.respond(Arrived.new) { reset }
							else
								@env.respond(Cancelled.new) { reset }
							end
						end
					end
				end
				@env.ack
			end
		else raise "FormationPathFinder cannot respond to #{env.msg}"
		end
	end

  private

    def reset
		@env, @dest = nil
	end
  
	class FormationGradient
		def initialize(grad, lead, dest, offset, speed)
			@grad = grad
			@lead = lead
			@dest = dest
			@offset = offset
			@speed = speed
		end

		def get_weight(pos)
			center = @lead.unit.get_position
			curweight = @grad.get_weight(pos)
			expweight = @grad.get_weight(center) + @grad.get_weight(@dest + @offset)
			if curweight < expweight
				(center + @offset - pos).norm / @speed 
			else
				curweight
			end
		end
	end
end
