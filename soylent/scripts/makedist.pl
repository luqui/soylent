#!/usr/bin/perl

sub usage {
    die "usage: makedist.pl win|linux <project name>\n".
        "                   <unix executable> <windows executable>\n".
	"                   <data files/directories>\n"
}

sub vsystem {
    print "@_\n";
    system "@_" and die "Command '@_' failed\n";
}

usage unless @ARGV >= 4;
my ($target, $project, $unixexec, $winexec, @data) = @ARGV;

if ($target eq 'win') {
    $ENV{PATH} = "/opt/xmingw/bin:/opt/xmingw/i386-mingw32msvc/bin:$ENV{PATH}";
    $ENV{CC}   = "i386-mingw32msvc-gcc";
    $ENV{CXX}  = "i386-mingw32msvc-g++";
    delete $ENV{CPPFLAGS};
    delete $ENV{LDFLAGS};
    $ENV{CFLAGS} = "-I/opt/xmingw/i386-mingw32msvc/include";
    $ENV{CXXFLAGS} = $ENV{CFLAGS};
    $ENV{BUILD_WINDOWS} = '1';
}
elsif ($target eq 'linux') {
    $ENV{CC}  = "gcc";
    $ENV{CXX} = "g++";
    delete $ENV{qw{CFLAGS CPPFLAGS LDFLAGS CXXFLAGS BUILD_WINDOWS}};
}
else {
    usage;
}

$ENV{BUILD_DEBUG} = '-DNDEBUG';  # build a release version

vsystem 'make clean all';
vsystem "rm -rf $project";
vsystem "mkdir $project";
for (@data) {
    vsystem "cp -r $_ $project/";
    vsystem "rm -rf $project/$_/.svn" if -d "$project/$_";
}

if ($target eq 'win') {
    vsystem "cp $unixexec $project/$winexec";
    vsystem "zip -r $project $project";
}
elsif ($target eq 'linux') {
    vsystem "cp $unixexec $project/$unixexec";
    vsystem "tar -zc $project > $project.tar.gz";
}
vsystem "rm -rf $project";
