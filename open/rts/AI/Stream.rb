# = Stream - the hairy guts behind the order protocol
# Envelope, Stream, AnonStream, PollDeliverStream are the classes in
# this file.

# Envelope is a container for a message, a.k.a. an order.  It holds a
# continuation into the commander so that when the order is completed,
# the commander can resume properly.  Envelopes are what are delivered
# to order methods; they are to be used by recievers, not senders,
# of orders.

class Envelope
	# msg - the message inside the envelope
	attr_reader :msg
	
	# This class is not usually created by user code.  Instead, use
	# Stream::env to create one from a stream.
	
	def initialize(cont, msg)
		@yield = cont
		@msg = msg
	end

	# Acknowledge the reciept of this order.  This is different from reporting
	# success (or failure), which is done through the respond method.  Every
	# order must be acknowledged (unless an error occurs, such as delivering an
	# unknown message), and an order must be acknowledged before it is
	# responded to.  The latter of these errors is caught nicely, but the
	# former can get you in a lot of debugging-hell trouble.
	#
	#   def order(env)
	#   	if MoveTo === env.msg
	#   		set_destination env.msg.dest
	#   		env.ack
	#   	else raise "Order not understood"
	#   end		
	
	def ack
		callcc do |cont|
			@continue = cont
			@yield[0].call
		end
	end

	# Respond to an order, which must have already been acknowledged with ack.
	# The response should be an instance of AI::Success or AI::Failure.  Typically
	# an order can only succeed in one way, so keep than in mind when you are
	# designing your response classes.  (However, information-gathering orders
	# may be able to succeed in multiple ways).  You should respond to every
	# order you are given exactly once (not doing so may leave your commanders
	# in limbo half-way through a plan).
	#
	# If there are state changes involved while responding to an order, you should
	# change all state *before* responding, so that if your commander decides to
	# order you to do something else immediately upon receiving your response, his
	# change will be noticed.  As a guideline, just make responding the last thing
	# you do in your order or step methods.
	#
	# As a convenience to encourage this guideline, this method takes an optional
	# block which is executed *before* the response is carried out.  This is so
	# you can send a cancel and replace your envelope without using any temporaries
	# like so:
	#
	#   def order(env)
	#   	if MoveTo === env.msg
	#   		@env.respond(Cancelled.new) do
	#   			@env = env
	#   			set_destination env.msg.dest
	#   			@env.ack
	#   		end
	#   	end
	#   end
	
	def respond(ans)
		yield if block_given?  # makes it easy to respond after ack, as good practice dictates
		callcc do |cont|
			@yield[0] = cont
			temp = @continue
			@yield = @continue = nil
			if temp
				temp.call(ans)
			else
				raise "Either you did not ack this message, " +
					  "or you responded to it twice"
			end
		end
	end
end

class << nil
	# nil supports respond, to help enable the idiom in Envelope::respond
	def respond(ans)
		yield if block_given?
	end
end

module Stream
	attr_writer :chain

	# It is *essential* that subclass initializers call super.  Not
	# doing so will cause major control flow issues.
	def initialize(*pass)
		super *pass
		@chain = proc { |x,r| x.call(r) }
	end

	# Begin running the stream.  This method is not meant to be 
	# overridden; instead use the 'stream' template method.
	def run
		callcc do |cont|
			@yield = [cont]   # hacky reference
			ret = stream
			@chain.call(@yield[0], ret)
		end
	end
	
	# Create an envelope for the given message and return it.  This
	# is how a stream sends messages.
	# 
	#   child.order(env(PathFinder::MoveTo(dest)))
	# 
	# For forked calls (with parallel_and and parallel_or), the
	# parent's env should not be used.  Instead use the block's
	# parameter's env.  See parallel_and for a description.
	
	def env(msg)
		Envelope.new(@yield, msg)
	end

  protected

	# Execute several order streams and wait until they all finish
	# before returning.  This method is given an array of blocks.  The
    # blocks take one parameter: the anonymous stream that they
    # are executed in.  When sending orders from these streams,
    # you should use the env of that anonymous stream, not of
    # parallel_and's caller.  For example:
    #
    #   child.order(env(some_message))  # good
    #   parallel_and [
    #     proc { |s| child.order(env(some_message)) }, # XXX WRONG
    #     proc { |s| child.order(s.env(some_message)) }, # correct
    #   ]
    #
    # The return value is an array of the return values of each
    # stream in order.
     
	def parallel_and(procs)
		ret = []
		if procs.length > 0
			count = 0
			y,r = parallel(procs)  # this will return multiple times
			count += 1
			@yield[0] = y          # I don't understand this line
			ret[r[0]] = r[1]
			if count < procs.length
				y.call
			end
		end
		return ret
	end

	# Execute several streams and wait until one of them finishes
	# before returning.  The return value is that of the stream
	# that finished.  See parallel_and for notes about parallel
	# calls -- all caveats stated there apply here as well.
	
	def parallel_or(procs)
		if procs.length > 0
			count = 0
			y,r = parallel(procs)  # this will return multiple times
			count += 1
			@yield[0] = y
			if count > 1
				y.call
			else
				return r[1]
			end
		end
		return nil
	end

	# Sends msg to target and immediately returns.  When the message
	# is responded to, the given block will be executed with the
	# result.
	#
	#   async(child, message) { |result| env.respond(result) }
	
	def async(target, msg, &callback)
		AsyncStream.new(target, msg, &callback).run
	end

  private

    # procs has to have at least length 1
	def parallel(procs)
		if procs.length == 0
			raise "Called parallel() with an empty list!"
		end

		callcc do |cont|
			for i in 0..procs.length-2
				str = AnonStream.new { [i, procs[i].call(str)] }
				str.chain = cont
				str.run
			end
			
			ret = procs[procs.length-1].call(self)
			return [@yield[0], [procs.length-1, ret]]
		end
	end
	
end

class AnonStream
	include Stream
	
	def initialize(&s)
		super()
		@str = s
	end

	def stream
		@str.call(self)
	end
end

class PollDeliverStream
	include Stream
	
	attr_reader :result
	
	def initialize(target, msg)
		super()
		@target = target
		@msg = msg
		@result = nil
	end

	def stream
		@result = [@target.order(env(@msg))]
	end	

	def responded?
		!!@result
	end

	def response
		raise "No response yet" unless responded?
		@result[0]
	end
end

class AsyncStream
	include Stream

	def initialize(target, msg, &callback)
		super()
		@target = target
		@msg = msg
		@callback = callback
	end

	def stream
		@callback.call(@target.order(env(@msg)))
	end
end
