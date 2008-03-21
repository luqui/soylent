require "Server"

exit if ARGV[0] == '-exy'

if ARGV.length > 0
	port = ARGV[0].to_i
else
	print "Port (4201)? "
	port = gets.chomp.to_i
	if port == 0
		port = 4201
	end
end

server = IPListener.new(port)
server.start
server.join
