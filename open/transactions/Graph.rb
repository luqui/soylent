require "set"

class Graph
	attr_accessor :artist
	
	def initialize
		@edges = Set.new
		@vertices = Set.new
		@vertex_ctr = 0
		@artist = nil
	end

	def add_edge(u,v)
		unless has_edge?(u,v) || u == v
			add_vertex(u)
			add_vertex(v)
			@edges.add([u,v])
		end
	end

	def add_vertex(v = nil)
		if v
			@vertices.add(v)
			v
		else
			while @vertices.member?(@vertex_ctr)
				@vertex_ctr += 1
			end
			@vertices.add(@vertex_ctr)
			vtx = @vertex_ctr
			@vertex_ctr += 1
			vtx
		end
	end

	def delete_edge(u,v)
		@edges.delete([u,v])
		@edges.delete([v,u])
	end

	def delete_vertex(v)
		@vertices.delete(v)
		@edges.delete_if { |a,b| a == v || b == v }
	end
	
	def has_edge?(u,v)
		@edges.member?([u,v]) || @edges.member?([v,u])
	end

	def has_vertex?(v)
		@vertices.member?(v)
	end

	def edges
		@edges
	end

	def vertices
		@vertices
	end
end
