programs = {
	'game-server' => 'telegnosis-server',
	'game-client' => 'telegnosis', 
	'game-editor' => 'telegnosis-editor',
}

def runprog(cmdline)
	puts "    #{cmdline}"
	stat = system cmdline
	unless stat
		puts "Exit status: #{stat} #$? #$!"
		raise
	end
end

programs.each do |src,target|
	puts "Creating #{target}.exe from #{src}.rb"
	puts "  Building #{src}.exy"
	runprog "ruby -r exerb/mkexy #{src}.rb -exy"
	puts "  Bulding #{target}.exe"
	case RUBY_PLATFORM
	when /win32/
		runprog "ruby C:\\ruby\\bin\\exerb -o #{target}.exe #{src}.exy"
	else
		runprog "exerb -o #{target}.exe #{src}.exy"
	end
end
