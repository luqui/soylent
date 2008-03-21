require "Editor"
require "Board"

exit if ARGV[0] == '-exy'

file = ARGV[0]
unless file
	file = Board.select_level :create
end

Editor.new(file).run
