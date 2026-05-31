#!/usr/bin/env perl

# run-test-suites.pl
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

=head1 SYNOPSIS

Execute all the test suites and print a summary of the results.

 run-test-suites.pl [[-v|--verbose] [VERBOSITY]] [--skip=SUITE[...]]

Options:

  -v|--verbose        Print detailed failure information.
  -v 2|--verbose=2    Print detailed failure information and summary messages.
  -v 3|--verbose=3    Print detailed information about every test case.
  --skip=SUITE[,SUITE...]
                      Skip the specified SUITE(s). This option can be used
                      multiple times.

=cut

use warnings;
use strict;

use utf8;
use open qw(:std utf8);

use Cwd qw(getcwd);
use Getopt::Long qw(:config auto_help gnu_compat);
use Pod::Usage;

my $verbose = 0;
my @skip_patterns = ();
GetOptions(
           'skip=s' => \@skip_patterns,
           'verbose|v:1' => \$verbose,
          ) or die;

# All test suites = executable files with a .datax file.
my @suites = ();
my @test_dirs = qw(../tf-psa-crypto/tests .);
for my $data_file (map {glob "$_/test_suite_*.datax"} @test_dirs) {
    (my $base = $data_file) =~ s/\.datax$//;
    push @suites, $base if -x $base;
    push @suites, "$base.exe" if -e "$base.exe";
}
die "$0: no test suite found\n" unless @suites;

# "foo" as a skip pattern skips "test_suite_foo" and "test_suite_foo.bar"
# but not "test_suite_foobar".
my $skip_re =
    ( '\Atest_suite_(' .
      join('|', map {
          s/[ ,;]/|/g; # allow any of " ,;|" as separators
          s/\./\./g; # "." in the input means ".", not "any character"
          $_
      } @skip_patterns) .
      ')(\z|\.)' );

# in case test suites are linked dynamically
$ENV{'LD_LIBRARY_PATH'} = getcwd() . "/../library";
$ENV{'DYLD_LIBRARY_PATH'} = $ENV{'LD_LIBRARY_PATH'}; # For macOS

my $prefix = $^O eq "MSWin32" ? '' : './';

my (@failed_suites, $total_tests_run, $failed, $suite_cases_passed,
    $suite_cases_failed, $suite_cases_skipped, $total_cases_passed,
    $total_cases_failed, $total_cases_skipped );
my $suites_skipped = 0;

sub pad_print_center {
    my( $width, $padchar, $string ) = @_;
    my $padlen = ( $width - length( $string ) - 2 ) / 2;
    print $padchar x( $padlen ), " $string ", $padchar x( $padlen ), "\n";
}

for my $suite_path (@suites)
{
    my ($dir, $suite) = ('.', $suite_path);
    if ($suite =~ m!(.*)/([^/]*)!) {
        $dir = $1;
        $suite = $2;
    }
    print "$suite ", "." x ( 72 - length($suite) - 2 - 4 ), " ";
    if( $suite =~ /$skip_re/o ) {
        print "SKIP\n";
        ++$suites_skipped;
        next;
    }

    my $command = "cd $dir && $prefix$suite";
    if( $verbose ) {
        $command .= ' -v';
    }
    my $result = `$command`;

    $suite_cases_passed = () = $result =~ /.. PASS/g;
    $suite_cases_failed = () = $result =~ /.. FAILED/g;
    $suite_cases_skipped = () = $result =~ /.. ----/g;

    if( $? == 0 ) {
        print "PASS\n";
        if( $verbose > 2 ) {
            pad_print_center( 72, '-', "Begin $suite" );
            print $result;
            pad_print_center( 72, '-', "End $suite" );
        }
    } else {
        push @failed_suites, $suite;
        print "FAIL\n";
        if( $verbose ) {
            pad_print_center( 72, '-', "Begin $suite" );
            print $result;
            pad_print_center( 72, '-', "End $suite" );
        }
    }

    my ($passed, $tests, $skipped) = $result =~ /([0-9]*) \/ ([0-9]*) tests.*?([0-9]*) skipped/;
    $total_tests_run += $tests - $skipped;

    if( $verbose > 1 ) {
        print "(test cases passed:", $suite_cases_passed,
                " failed:", $suite_cases_failed,
                " skipped:", $suite_cases_skipped,
                " of total:", ($suite_cases_passed + $suite_cases_failed +
                               $suite_cases_skipped),
                ")\n"
    }

    $total_cases_passed += $suite_cases_passed;
    $total_cases_failed += $suite_cases_failed;
    $total_cases_skipped += $suite_cases_skipped;
}

print "-" x 72, "\n";
print @failed_suites ? "FAILED" : "PASSED";
printf( " (%d suites, %d tests run%s)\n",
        scalar(@suites) - $suites_skipped,
        $total_tests_run,
        $suites_skipped ? ", $suites_skipped suites skipped" : "" );

if( $verbose && @failed_suites ) {
    # the output can be very long, so provide a summary of which suites failed
    print "      failed suites : @failed_suites\n";
}

if( $verbose > 1 ) {
    print "  test cases passed :", $total_cases_passed, "\n";
    print "             failed :", $total_cases_failed, "\n";
    print "            skipped :", $total_cases_skipped, "\n";
    print "  of tests executed :", ( $total_cases_passed + $total_cases_failed ),
            "\n";
    print " of available tests :",
            ( $total_cases_passed + $total_cases_failed + $total_cases_skipped ),
            "\n";
    if( $suites_skipped != 0 ) {
        print "Note: $suites_skipped suites were skipped.\n";
    }
}

exit( @failed_suites ? 1 : 0 );

