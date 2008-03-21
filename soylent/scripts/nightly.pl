#!/usr/bin/perl

use List::Util qw<max>;

my ($force) = @ARGV;

my $root = '/home/reidn';
my %UPDATED;

sub vsystem {
    print "@_\n";
    open my $fh, '-|', "@_";
    my @ret;
    local $_;
    while (<$fh>) {
        print;
        push @ret, $_;
    }
    close $fh or die "Command @_ failed\n";
    return @ret;
}

sub modified_time {
    my $mod = max(map { "".localtime((stat $_)[9]) } @_);
    $mod =~ s/..:..:.. //;
    return $mod;
}

=pod

print "Minesweeper\n";
print "-----------\n";
chdir "$root/soylent/soylent/minesweeper_infinity";
MINESWEEPER: 
for (1) {
    my @uplines = vsystem 'svn update';
    my $info = join "\n", vsystem 'svn info';
    ($UPDATED{MINESWEEPER}) = ($info =~ /^Last Changed Date: [^\n]*\((.*?)\)/m);
    print "Minesweeper was updated $UPDATED{MINESWEEPER}\n";
    if (@uplines == 1 && $force ne '-force') { 
        print "No updates; we don't need to rebuild\n";
        last MINESWEEPER;
    }

    vsystem "perl $root/soylent/soylent/scripts/makedist.pl linux super_minesweeper mines 'Super Minesweeper.exe' art";
    # Windows build is broken :-(
#    vsystem "perl $root/soylent/soylent/scripts/makedist.pl win   super_minesweeper mines 'Super Minesweeper.exe' art";

    # vsystem "cp minesweeper_infinity.zip $root/public_html";
    vsystem "cp minesweeper_infinity.tar.gz $root/public_html";
}

print "Fluid\n";
print "-----------\n";
chdir "$root/soylent/open/fluid";
FLUID:
for (1) {
    my @uplines = vsystem 'svn update';
    my $info = join "\n", vsystem 'svn info';
    ($UPDATED{FLUID}) = ($info =~ /^Last Changed Date: [^\n]*\((.*?)\)/m);
    print "fluid was updated $UPDATED{FLUID}\n";
    if (@uplines == 1 && $force ne '-force') { 
        print "No updates; we don't need to rebuild\n";
        last FLUID;
    }

    vsystem "perl $root/soylent/soylent/scripts/makedist.pl linux fluid fluid Fluid.exe energy.png firefly.png";
    # Windows build is broken :-(
    # vsystem "perl $root/soylent/soylent/scripts/makedist.pl win   fluid fluid Fluid.exe firefly-down.png firefly-up.png";
}

=cut

print "\nWebsite\n";
print "-------\n";
chdir "$root/public_html";

$UPDATED{MINESWEEPER}  = modified_time('minesweeper_infinity.tar.gz', 'minesweeper_infinity.zip');
$UPDATED{FLAMETHROWER} = modified_time('ichor-0.1.tar.gz');

{
    vsystem 'svn update';
    
    my ($inf, $outf);
    open $inf, '<', 'index.html.in' or die "Couldn't open index.html.in: $!";
    open $outf, '>', 'index.html' or die "Couldn't open index.html: $!";
    
    while (<$inf>) {
        s/<!--UPDATE-(\w+)-->/$UPDATED{$1}/e;
        print $outf $_;
    }
}
