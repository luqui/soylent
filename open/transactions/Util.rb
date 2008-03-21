module Util
	SCREEN_WIDTH = 128.0
	SCREEN_HEIGHT = 96.0

	CHAR_WIDTH = 9
	CHAR_HEIGHT = 15
	case RUBY_PLATFORM
	when /win32/		
		def Util::draw_string(str)
			str.each_byte do |ch|
				GLUT.BitmapCharacter(2, ch)
			end
		end
	else
		def Util::draw_string(str)
			str.each_byte do |ch|
				GLUT.BitmapCharacter(GLUT::BITMAP_9_BY_15, ch)
			end
		end
	end

	def Util::layout(ll, ur)
		GL.MatrixMode GL::PROJECTION
		GL.PushMatrix
		GL.LoadIdentity
		GLU.Ortho2D ll.x, ur.x, ll.y, ur.y
		GL.MatrixMode GL::MODELVIEW
		yield
		GL.MatrixMode GL::PROJECTION
		GL.PopMatrix
		GL.MatrixMode GL::MODELVIEW
	end

	def Util::scr_char_height
		CHAR_HEIGHT * SCREEN_HEIGHT / GLUT.Get(GLUT::WINDOW_HEIGHT)
	end

	def Util::scr_char_width
		CHAR_WIDTH * SCREEN_WIDTH / GLUT.Get(GLUT::WINDOW_WIDTH)
	end

	def Util::box_vertices(ll, ur)
		GL.Vertex ll.x, ll.y
		GL.Vertex ll.x, ur.y
		GL.Vertex ur.x, ur.y
		GL.Vertex ur.x, ll.y
	end

	def Util::draw_circle
		GL.Begin GL::LINE_LOOP
		24.times do |i|
			theta = 2 * Math::PI * i / 24
			GL.Vertex Math.cos(theta), Math.sin(theta)
		end
		GL.End
	end

	def Util::draw_disc
		GL.Begin GL::TRIANGLE_FAN
		GL.Vertex 0,0
		(0..24).each do |i|
			theta = 2 * Math::PI * i / 24
			GL.Vertex Math.cos(theta), Math.sin(theta)
		end
		GL.End
	end

	# takes an array of two-element arrays of the form: [ color, text ]
	def Util::draw_text_box(halign, hdist, valign, vdist, text)
		# Draw bounding box
		return if text.length == 0
		height = text.length
		maxwidth = text.map { |c,t| t.length }.max

		case halign
		when :left
			l, r = hdist, 2 + hdist + maxwidth * scr_char_width
		when :right
			l, r = SCREEN_WIDTH - (2 + hdist + maxwidth * scr_char_width), SCREEN_WIDTH - hdist
		else raise "halign must be :left or :right, not #{halign.inspect}"
		end
		
		case valign
		when :bottom
			b, t = vdist, 1.5 + vdist + 1.5 * height * scr_char_height
		when :top
			b, t = SCREEN_HEIGHT - (1.5 + vdist + 1.5 * height * scr_char_height), SCREEN_HEIGHT - vdist
		else raise "valign must be :bottom or :top, not #{valign.inspect}"
		end

		GL.Color 0.15, 0.15, 0.15, 0.7
		GL.Begin GL::QUADS
		Util.box_vertices Vec.new(l,b), Vec.new(r,t)
		GL.End

		# Draw text
		ypos = t
		text.each do |c,t|
			ypos -= 1.5 * scr_char_height
			GL.Color *c
			GL.RasterPos l + 1, ypos
			draw_string t
		end
	end
end
