class AwakenException < Exception; end

class GlutSucksTimer
	include MonitorMixin
	
	def initialize
		super
		@timers = []
		@thread = Thread.new do
			begin
				run
			rescue => x
				puts "EXCEPTION: #{x.message} #{x.backtrace.inspect}"
				raise
			end
		end
	end

	def delay(delta, &code)
		synchronize {
		target = Time.new + delta
		if @timers.length == 0
			@timers.push [target, code]
			@thread.raise(AwakenException.new)
		elsif target < @timers[0][0]
			# this is a separate case because in this case
			# we need to wakeup
			@timers.unshift [target, code]
			@thread.raise(AwakenException.new)
		else
			idx = 0
			@timers.each do |t,c|
				break if target < t
				idx += 1
			end
			@timers.insert(idx, [target, code])
		end
		}
	end

	def run
		while true
			begin
				sleep *synchronize {
					time = Time.now
					while @timers.length > 0 && time >= @timers[0][0]
						@timers.shift()[1].call
					end

					sleeptime = if @timers.length > 0
						timediff = @timers[0][0] - time
						timediff = 0 if timediff < 0
						[timediff]
					else
						[]
					end
					sleeptime
				}
			rescue AwakenException => x
			end
		end
	end
end
