class Coroutine
	def initialize(&block)
		args = callcc do |cc|
			@resume = cc
			return
		end

		@suspend.call(*block.call(*args))
	end

	def call(*args)
		callcc do |cc|
			@suspend = cc
			@resume.call(*args)
		end
	end

	def yield(*args)
		callcc do |cc|
			@resume = cc
			@suspend.call(*args)
		end
	end
end
