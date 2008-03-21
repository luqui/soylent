# = boot.rb - Load required modules and set up
#
# This is the boot file for the main game.  The Ruby side should do whatever it
# needs to do to initialize here, then call main_game_loop which calls back
# into the mainloop in C++.

$LOAD_PATH << File.expand_path(File.dirname(__FILE__))

require "Vec"
require "AI/AI"
require "AI/Stream"
require "AI/LinearMover"
require "AI/PathFinder"
require "AI/FormationPathFinder"

main_game_loop
