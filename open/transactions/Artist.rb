require "opengl"
require "glut"
require "Util"

class GraphArtist
	def initialize(graph)
		@nodepos = {}
		@graph = graph
	end

	def set_pos(vertex, pos)
		@nodepos[vertex] = pos
	end

	def get_pos(vertex)
		@nodepos[vertex]
	end

	def prune
		@nodepos.delete_if { |v,p| !@graph.has_vertex?(v) }
	end

	def pick_vertex(pos)
		@graph.vertices.each do |v|
			vpos = @nodepos[v]
			if (pos - vpos).norm < 2
				return v
			end
		end
		nil
	end

	def pick_edge(pos)
		@graph.edges.each do |a,b|
			apos, bpos = @nodepos[a], @nodepos[b]
			if (pos - apos) * (bpos - apos) > 0 &&   # on b's side of a
			   (pos - bpos) * (apos - bpos) > 0 &&   # on a's side of b
			   ((~(bpos - apos)) % (pos - apos)).abs < 0.5  # very close to the line
					return [a,b]
			end
		end
		nil
	end
	
	def draw
		@graph.vertices.each do |v|
			pos = @nodepos[v]
			GL.PushMatrix
			GL.Translate pos.x, pos.y, 0
			GL.Color 1,1,1
			Util.draw_circle
			GL.PopMatrix
		end

		@graph.edges.each do |a,b|
			GL.Color 1,1,1
			draw_edge(a,b)
		end
	end
	
	def draw_edge(a,b)
		apos, bpos = @nodepos[a], @nodepos[b]
		apos += ~(bpos - apos)
		bpos += ~(apos - bpos)
		GL.Begin GL::LINES
		GL.Vertex apos.x(), apos.y()
		GL.Vertex bpos.x, bpos.y
		GL.End
	end
end

DEFAULT_SCHEME = {
	0 => [1.0, 0.5, 0.0, 1],
	1 => [0.0, 0.5, 1.0, 1],
	2 => [1.0, 1.0, 0.0, 1],
	3 => [1.0, 0.0, 1.0, 1],
	4 => [0.5, 1.0, 0.0, 1],
	5 => [1.0, 0.0, 0.0, 1],
	6 => [0.5, 0.5, 0.5, 1],
}

class BoardArtist
	def initialize(board, graph_artist = nil)
		@board = board
		@graph_artist = graph_artist || @board.graph.artist
	end
	
	def draw_graph
		@graph_artist.draw
	end
	
	def draw(scheme = DEFAULT_SCHEME)
		@board.units.each do |loc,player|
			pos = @graph_artist.get_pos(loc)
			GL.PushMatrix
			GL.Translate pos.x, pos.y, 0
			GL.Color *(scheme[player])
			Util.draw_disc
			GL.PopMatrix
		end

		@board.supply.each do |loc,player|
			pos = @graph_artist.get_pos(loc)
			GL.PushMatrix
			GL.Translate pos.x, pos.y, 0
			GL.Scale 1.35, 1.35, 1
			GL.Color *(scheme[player])
			GL.LineWidth 2.0
			Util.draw_circle
			GL.LineWidth 1.0
			GL.PopMatrix
		end
	end
end
