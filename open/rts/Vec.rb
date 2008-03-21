# = Vec - Reimplementation of vec2.h from soylib

class Vec
	attr_reader :x, :y

	#   vector = Vec.new(x,y)
	
	def initialize(x = 0.0, y = 0.0)
		@x = x.to_f
		@y = y.to_f
	end

	def to_s
		"(" + @x.to_s + ", " + @y.to_s + ")"
	end

	# Takes the floor of each component and returns the result
	# in a new vector.
	#
	#   Vec.new(3.7, 2.4).quantize == Vec.new(3,2)
	
	def quantize
		Vec.new(@x.floor.to_f, @y.floor.to_f)
	end

	def x= (s)
		@x = s.to_f
	end

	def y= (s)
		@y = s.to_f
	end
	
	# Checks for *exact* equality.  It is unwise to use this without
	# eg. quantizing first.
	
	def == (v)
		@x == v.x && @y == v.y
	end

	def + (v)
		Vec.new(@x + v.x, @y + v.y)
	end

	def - (v)
		Vec.new(@x - v.x, @y - v.y)
	end

	def -@
		Vec.new(-@x, -@y)
	end

	# Dot product or scalar multiplication:
	# 
	#   Vec.new(2,3) * Vec.new(2,3)  == 13
	#   Vec.new(2,3) * 4             == Vec.new(8,12)
	#   4 * Vec.new(2,3)   # error, order of arguments!

	def * (v)
		case v
		when Vec then @x * v.x + @y * v.y
		else Vec.new(@x*v, @y*v)
		end
	end

	def / (s)
		Vec.new(@x/s, @y/s)
	end

	# 2D Cross product
	# 
	#   Vec.new(1,2) % Vec.new(3,4) == -2
	
	def % (v)
		@x * v.y - @y * v.x
	end

	# The length squared.  See norm.
	
	def norm2
		@x*@x + @y*@y
	end

	# Otherwise known as length.  
	#
	#   Vec2.new(3,4).norm == 5
	
	def norm
		Math.sqrt(norm2)
	end
	
	# Returns the vector normalized.
	#
	#   ~Vec2.new(1,1) == Vec2.new(1,1) / Math.sqrt(2)
	
	def ~
		self / norm
	end

	def Vec::average_vec(vecs)
		ret = Vec.new(0,0)
		vecs.each { |v| ret += v }
		ret / vecs.length
	end
end
