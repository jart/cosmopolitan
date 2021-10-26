#!/usr/bin/env perl
# -*-perl-*-
#
# This script helps us write tests in a portable way, without relying on a lot
# of shell features.  Since we already have Perl to run the tests, use that.
#
# The arguments represent a set of steps that will be run one at a time.
# Each step consists of an operator and argument.
#
# It supports the following operators:
#  out <word>   : echo <word> to stdout
#  file <word>  : echo <word> to stdout AND create the file <word>
#  dir <word>   : echo <word> to stdout AND create the directory <word>
#  rm <word>    : echo <word> to stdout AND delete the file/directory <word>
#  wait <word>  : wait for a file named <word> to exist
#  tmout <secs> : Change the timeout for waiting.  Default is 4 seconds.
#  sleep <secs> : Sleep for <secs> seconds then echo <secs>
#  fail <err>   : echo <err> to stdout then exit with error code err
#
# If given -q only the "out" command generates output.

# Force flush
$| = 1;

my $quiet = 0;
my $timeout = 4;

sub op {
    my ($op, $nm) = @_;

    defined $nm or die "Missing value for $op\n";

    if ($op eq 'out') {
        print "$nm\n";
        return 1;
    }

    # Show the output before creating the file
    if ($op eq 'file') {
        print "file $nm\n" unless $quiet;
        open(my $fh, '>', $nm) or die "$nm: open: $!\n";
        close(my $fh);
        return 1;
    }

    # Show the output before creating the directory
    if ($op eq 'dir') {
        print "dir $nm\n" unless $quiet;
        mkdir($nm) or die "$nm: mkdir: $!\n";
        return 1;
    }

    # Show the output after removing the file
    if ($op eq 'rm') {
        if (-f $nm) {
            unlink($nm) or die "$nm: unlink: $!\n";
        } elsif (-d $nm) {
            rmdir($nm) or die "$nm: rmdir: $!\n";
        } else {
            die "$nm: not file or directory: $!\n";
        }
        print "rm $nm\n" unless $quiet;
        return 1;
    }

    if ($op eq 'tmout') {
        $timeout = $nm;
        print "tmout $nm\n" unless $quiet;
        return 1;
    }

    # Show the output after the file exists
    if ($op eq 'wait') {
        my $start = time();
        my $end = $start + $timeout;
        while (time() <= $end) {
            if (-f $nm) {
                print "wait $nm\n" unless $quiet;
                return 1;
            }
            select(undef, undef, undef, 0.1);
        }
        die "wait $nm: timeout after ".(time()-$start-1)." seconds\n";
    }

    # Show the output after sleeping
    if ($op eq 'sleep') {
        sleep($nm);
        print "sleep $nm\n" unless $quiet;
        return 1;
    }

    if ($op eq 'fail') {
        print "fail $nm\n";
        exit($nm);
    }

    die("Invalid command: $op $nm\n");
}

if (@ARGV && $ARGV[0] eq '-q') {
    $quiet = 1;
    shift;
}

while (@ARGV) {
    if (op($ARGV[0], $ARGV[1])) {
        shift;
        shift;
    }
}

exit(0);
