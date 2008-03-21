require "Server"

exit if ARGV[0] == '-exy'

if ARGV.length > 0
	name = ARGV[0]
	ip = ARGV[1] || 'soylentsoft.net'
	port = (ARGV[2] || 4201).to_i
else
	print "What's your name? "
	name = gets.chomp
	if name == ""
		case RUBY_PLATFORM
		when /win32/
			name = ENV['USERNAME']
		else
			name = ENV['USER']
		end
	end

	print "Connect to what server (soylentsoft.net)? "
	ip = gets.chomp
	ip = 'soylentsoft.net' if ip == ""

	print "Port (4201)? "
	port = gets.chomp.to_i
	port = 4201 if port == 0
end

puts "Connecting to #{ip}:#{port} as #{name}"

client = Browser.new(name, ip, port)
client.run
