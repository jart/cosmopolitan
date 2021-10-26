#!/usr/bin/perl
# -*-perl-*-
#
# Modification history:
# Written 91-12-02 through 92-01-01 by Stephen McGee.
# Modified 92-02-11 through 92-02-22 by Chris Arthur to further generalize.
#
# Copyright (C) 1991-2020 Free Software Foundation, Inc.
# This file is part of GNU Make.
#
# GNU Make is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 3 of the License, or (at your option) any later
# version.
#
# GNU Make is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>.


# Test driver routines used by a number of test suites, including
# those for SCS, make, roll_dir, and scan_deps (?).
#
# this routine controls the whole mess; each test suite sets up a few
# variables and then calls &toplevel, which does all the real work.

# $Id$

use Config;
use Cwd;
use File::Spec;

# The number of test categories we've run
$categories_run = 0;
# The number of test categroies that have passed
$categories_passed = 0;
# The total number of individual tests that have been run
$total_tests_run = 0;
# The total number of individual tests that have passed
$total_tests_passed = 0;
# The number of tests in this category that have been run
$tests_run = 0;
# The number of tests in this category that have passed
$tests_passed = 0;


# Yeesh.  This whole test environment is such a hack!
$test_passed = 1;

# Timeout in seconds.  If the test takes longer than this we'll fail it.
$test_timeout = 5;
$test_timeout = 10 if $^O eq 'VMS';

# Path to Perl
$perl_name = $^X;
if ($^O ne 'VMS') {
    $perl_name .= $Config{_exe} unless $perl_name =~ m/$Config{_exe}$/i;
}
# If it's a simple name, look it up on PATH
{
    my ($v,$d,$f) = File::Spec->splitpath($perl_name);
    if (!$d) {
        my $perl = undef;
        foreach my $p (File::Spec->path()) {
            my $f = File::Spec->catfile($p, $f);
            if (-e $f) {
                $perl = $f;
                last;
            }
        }
        if ($perl) {
            $perl_name = $perl;
        } else {
            print "Cannot locate Perl interpreter $perl_name\n";
        }
    }
}
# Make sure it uses forward-slashes even on Windows, else it won't work
# in recipes
$perl_name =~ tr,\\,/,;

# %makeENV is the cleaned-out environment.
%makeENV = ();

# %extraENV are any extra environment variables the tests might want to set.
# These are RESET AFTER EVERY TEST!
%extraENV = ();

sub vms_get_process_logicals {
  # Sorry for the long note here, but to keep this test running on
  # VMS, it is needed to be understood.
  #
  # Perl on VMS by default maps the %ENV array to the system wide logical
  # name table.
  #
  # This is a very large dynamically changing table.
  # On Linux, this would be the equivalent of a table that contained
  # every mount point, temporary pipe, and symbolic link on every
  # file system.  You normally do not have permission to clear or replace it,
  # and if you did, the results would be catastrophic.
  #
  # On VMS, added/changed %ENV items show up in the process logical
  # name table.  So to track changes, a copy of it needs to be captured.

  my $raw_output = `show log/process/access_mode=supervisor`;
  my @raw_output_lines = split('\n',$raw_output);
  my %log_hash;
  foreach my $line (@raw_output_lines) {
    if ($line =~ /^\s+"([A-Za-z\$_]+)"\s+=\s+"(.+)"$/) {
      $log_hash{$1} = $2;
    }
  }
  return \%log_hash
}

# %origENV is the caller's original environment
if ($^O ne 'VMS') {
  %origENV = %ENV;
} else {
  my $proc_env = vms_get_process_logicals;
  %origENV = %{$proc_env};
}

sub resetENV
{
  # We used to say "%ENV = ();" but this doesn't work in Perl 5.000
  # through Perl 5.004.  It was fixed in Perl 5.004_01, but we don't
  # want to require that here, so just delete each one individually.

  if ($^O ne 'VMS') {
    foreach $v (keys %ENV) {
      delete $ENV{$v};
    }

    %ENV = %makeENV;
  } else {
    my $proc_env = vms_get_process_logicals();
    my %delta = %{$proc_env};
    foreach my $v (keys %delta) {
      if (exists $origENV{$v}) {
        if ($origENV{$v} ne $delta{$v}) {
          $ENV{$v} = $origENV{$v};
        }
      } else {
        delete $ENV{$v};
      }
    }
  }

  foreach $v (keys %extraENV) {
    $ENV{$v} = $extraENV{$v};
    delete $extraENV{$v};
  }
}

sub toplevel
{
  # Pull in benign variables from the user's environment

  foreach (# UNIX-specific things
           'TZ', 'TMPDIR', 'HOME', 'USER', 'LOGNAME', 'PATH',
           'LD_LIBRARY_PATH',
           # SAN things
           'ASAN_OPTIONS', 'UBSAN_OPTIONS',
           # Purify things
           'PURIFYOPTIONS',
           # Windows NT-specific stuff
           'Path', 'SystemRoot',
           # DJGPP-specific stuff
           'DJDIR', 'DJGPP', 'SHELL', 'COMSPEC', 'HOSTNAME', 'LFN',
           'FNCASE', '387', 'EMU387', 'GROUP'
          ) {
    $makeENV{$_} = $ENV{$_} if $ENV{$_};
  }

  # Make sure our compares are not foiled by locale differences

  $makeENV{LC_ALL} = 'C';

  # Replace the environment with the new one
  #
  %origENV = %ENV unless $^O eq 'VMS';

  resetENV();

  $| = 1;                     # unbuffered output

  $debug = 0;                 # debug flag
  $profile = 0;               # profiling flag
  $verbose = 0;               # verbose mode flag
  $detail = 0;                # detailed verbosity
  $keep = 0;                  # keep temp files around
  $workdir = "work";          # The directory where the test will start running
  $scriptdir = "scripts";     # The directory where we find the test scripts
  $tmpfilesuffix = "t";       # the suffix used on tmpfiles
  $default_output_stack_level = 0;  # used by attach_default_output, etc.
  $default_input_stack_level = 0;   # used by attach_default_input, etc.
  $cwd = ".";                 # don't we wish we knew
  $cwdslash = "";             # $cwd . $pathsep, but "" rather than "./"

  &get_osname;  # sets $osname, $vos, $pathsep, and $short_filenames

  &set_defaults;  # suite-defined

  &parse_command_line (@ARGV);

  print "OS name = '$osname'\n" if $debug;

  $workpath = "$cwdslash$workdir";
  $scriptpath = "$cwdslash$scriptdir";

  &set_more_defaults;  # suite-defined

  &print_banner;

  if ($osname eq 'VMS' && $cwdslash eq "") {
    # Porting this script to VMS revealed a small bug in opendir() not
    # handling search lists correctly when the directory only exists in
    # one of the logical_devices.  Need to find the first directory in
    # the search list, as that is where things will be written to.
    my @dirs = split('/', $cwdpath);

    my $logical_device = $ENV{$dirs[1]};
    if ($logical_device =~ /([A-Za-z0-9_]+):(:?.+:)+/) {
      # A search list was found.  Grab the first logical device
      # and use it instead of the search list.
      $dirs[1]=$1;
      my $lcl_pwd = join('/', @dirs);
      $workpath = $lcl_pwd . '/' . $workdir
    }
  }

  if (-d $workpath) {
    print "Clearing $workpath...\n";
    &remove_directory_tree("$workpath/")
        or &error ("Couldn't wipe out $workpath: $!\n");
  } else {
    mkdir ($workpath, 0777) or &error ("Couldn't mkdir $workpath: $!\n");
  }

  if (!-d $scriptpath) {
    &error ("Failed to find $scriptpath containing perl test scripts.\n");
  }

  if (@TESTS) {
    print "Making work dirs...\n";
    foreach $test (@TESTS) {
      if ($test =~ /^([^\/]+)\//) {
        $dir = $1;
        push (@rmdirs, $dir);
        -d "$workpath/$dir"
           or mkdir ("$workpath/$dir", 0777)
           or &error ("Couldn't mkdir $workpath/$dir: $!\n");
      }
    }
  } else {
    print "Finding tests...\n";
    opendir (SCRIPTDIR, $scriptpath)
        or &error ("Couldn't opendir $scriptpath: $!\n");
    @dirs = grep (!/^(\..*|CVS|RCS)$/, readdir (SCRIPTDIR) );
    closedir (SCRIPTDIR);
    foreach my $dir (@dirs) {
      next if ($dir =~ /^(\..*|CVS|RCS)$/ || ! -d "$scriptpath/$dir");
      push (@rmdirs, $dir);
      # VMS can have overlayed file systems, so directories may repeat.
      next if -d "$workpath/$dir";
      mkdir ("$workpath/$dir", 0777)
          or &error ("Couldn't mkdir $workpath/$dir: $!\n");
      opendir (SCRIPTDIR, "$scriptpath/$dir")
          or &error ("Couldn't opendir $scriptpath/$dir: $!\n");
      @files = grep (!/^(\..*|CVS|RCS|.*~)$/, readdir (SCRIPTDIR) );
      closedir (SCRIPTDIR);
      foreach my $test (@files) {
        -d $test and next;
        push (@TESTS, "$dir/$test");
      }
    }
  }

  if (@TESTS == 0) {
    &error ("\nNo tests in $scriptpath, and none were specified.\n");
  }

  print "\n";

  run_all_tests();

  foreach my $dir (@rmdirs) {
    rmdir ("$workpath/$dir");
  }

  $| = 1;

  $categories_failed = $categories_run - $categories_passed;
  $total_tests_failed = $total_tests_run - $total_tests_passed;

  if ($total_tests_failed) {
    print "\n$total_tests_failed Test";
    print "s" unless $total_tests_failed == 1;
    print " in $categories_failed Categor";
    print ($categories_failed == 1 ? "y" : "ies");
    print " Failed (See .$diffext* files in $workdir dir for details) :-(\n\n";
    return 0;
  }

  print "\n$total_tests_passed Test";
  print "s" unless $total_tests_passed == 1;
  print " in $categories_passed Categor";
  print ($categories_passed == 1 ? "y" : "ies");
  print " Complete ... No Failures :-)\n\n";
  return 1;
}

sub get_osname
{
  # Set up an initial value.  In perl5 we can do it the easy way.
  $osname = defined($^O) ? $^O : '';

  # find the type of the port.  We do this up front to have a single
  # point of change if it needs to be tweaked.
  #
  # This is probably not specific enough.
  #
  if ($osname =~ /MSWin32/i || $osname =~ /Windows/i
      || $osname =~ /MINGW32/i || $osname =~ /CYGWIN_NT/i) {
    $port_type = 'W32';
  }
  # Bleah, the osname is so variable on DOS.  This kind of bites.
  # Well, as far as I can tell if we check for some text at the
  # beginning of the line with either no spaces or a single space, then
  # a D, then either "OS", "os", or "ev" and a space.  That should
  # match and be pretty specific.
  elsif ($osname =~ /^([^ ]*|[^ ]* [^ ]*)D(OS|os|ev) /) {
    $port_type = 'DOS';
  }
  # Check for OS/2
  elsif ($osname =~ m%OS/2%) {
    $port_type = 'OS/2';
  }

  # VMS has a GNV Unix mode or a DCL mode.
  # The SHELL environment variable should not be defined in VMS-DCL mode.
  elsif ($osname eq 'VMS' && !defined $ENV{"SHELL"}) {
    $port_type = 'VMS-DCL';
  }
  # Everything else, right now, is UNIX.  Note that we should integrate
  # the VOS support into this as well and get rid of $vos; we'll do
  # that next time.
  else {
    $port_type = 'UNIX';
  }

  if ($osname eq 'VMS')
  {
    $vos = 0;
    $pathsep = "/";
    return;
  }

  # Find a path to Perl

  # See if the filesystem supports long file names with multiple
  # dots.  DOS doesn't.
  $short_filenames = 0;
  (open (TOUCHFD, "> fancy.file.name") and close (TOUCHFD))
      or $short_filenames = 1;
  unlink ("fancy.file.name") or $short_filenames = 1;

  if (! $short_filenames) {
    # Thanks go to meyering@cs.utexas.edu (Jim Meyering) for suggesting a
    # better way of doing this.  (We used to test for existence of a /mnt
    # dir, but that apparently fails on an SGI Indigo (whatever that is).)
    # Because perl on VOS translates /'s to >'s, we need to test for
    # VOSness rather than testing for Unixness (ie, try > instead of /).

    mkdir (".ostest", 0777) or &error ("Couldn't create .ostest: $!\n", 1);
    open (TOUCHFD, "> .ostest>ick") and close (TOUCHFD);
    chdir (".ostest") or &error ("Couldn't chdir to .ostest: $!\n", 1);
  }

  if (! $short_filenames && -f "ick") {
    $osname = "vos";
    $vos = 1;
    $pathsep = ">";

  } else {
    # the following is regrettably gnarly, but it seems to be the only way
    # to not get ugly error messages if uname can't be found.
    # Hmmm, BSD/OS 2.0's uname -a is excessively verbose.  Let's try it
    # with switches first.
    eval "chop (\$osname = `sh -c 'uname -nmsr 2>&1'`)";
    if ($osname =~ /not found/i) {
      $osname = "(something posixy with no uname)";

    } elsif ($@ ne "" || $?) {
      eval "chop (\$osname = `sh -c 'uname -a 2>&1'`)";
      if ($@ ne "" || $?) {
        $osname = "(something posixy)";
      }
    }
    $vos = 0;
    $pathsep = "/";
  }

  if (! $short_filenames) {
    chdir ("..") or &error ("Couldn't chdir to ..: $!\n", 1);
    unlink (".ostest>ick");
    rmdir (".ostest") or &error ("Couldn't rmdir .ostest: $!\n", 1);
  }
}

sub parse_command_line
{
  @argv = @_;

  # use @ARGV if no args were passed in

  if (@argv == 0) {
    @argv = @ARGV;
  }

  # look at each option; if we don't recognize it, maybe the suite-specific
  # command line parsing code will...

  while (@argv) {
    $option = shift @argv;
    if ($option =~ /^-usage$/i) {
      &print_usage;
      exit 0;
    }
    if ($option =~ /^-(h|help)$/i) {
      &print_help;
      exit 0;
    }

    if ($option =~ /^-debug$/i) {
      print "\nDEBUG ON\n";
      $debug = 1;

    } elsif ($option =~ /^-profile$/i) {
      $profile = 1;

    } elsif ($option =~ /^-verbose$/i) {
      $verbose = 1;

    } elsif ($option =~ /^-detail$/i) {
      $detail = 1;
      $verbose = 1;

    } elsif ($option =~ /^-keep$/i) {
      $keep = 1;

    } elsif (&valid_option($option)) {
      # The suite-defined subroutine takes care of the option

    } elsif ($option =~ /^-/) {
      print "Invalid option: $option\n";
      &print_usage;
      exit 0;

    } else { # must be the name of a test
      $option =~ s/\.pl$//;
      push(@TESTS,$option);
    }
  }
}

sub max
{
  my $num = shift @_;
  my $newnum;

  while (@_) {
    $newnum = shift @_;
    if ($newnum > $num) {
      $num = $newnum;
    }
  }

  return $num;
}

sub print_centered
{
  my ($width, $string) = @_;

  if (length ($string)) {
    my $pad = " " x ( ($width - length ($string) + 1) / 2);
    print "$pad$string";
  }
}

sub print_banner
{
  # $testee is suite-defined
  my $info = "Running tests for $testee on $osname\n";
  my $len = &max (length($info), length($testee_version), 73) + 5;
  my $line = ("-" x $len) . "\n";

  &print_centered ($len, $line);
  &print_centered ($len, $info);
  &print_centered ($len, $testee_version);
  &print_centered ($len, $line);
  print "\n";
}

sub run_all_tests
{
  # Make sure we always run the tests from the current directory
  unshift(@INC, cwd());

  $categories_run = 0;

  $lasttest = '';
  # $testname is published
  foreach $testname (sort @TESTS) {
    # Skip duplicates on VMS caused by logical name search lists.
    next if $testname eq $lasttest;
    $lasttest = $testname;
    $suite_passed = 1;       # reset by test on failure
    $num_of_logfiles = 0;
    $num_of_tmpfiles = 0;
    $description = "";
    $details = "";
    $old_makefile = undef;
    $testname =~ s/^$scriptpath$pathsep//;
    $perl_testname = "$scriptpath$pathsep$testname";
    $testname =~ s/(\.pl|\.perl)$//;
    $testpath = "$workpath$pathsep$testname";
    # Leave enough space in the extensions to append a number, even
    # though it needs to fit into 8+3 limits.
    if ($short_filenames) {
      $logext = 'l';
      $diffext = 'd';
      $baseext = 'b';
      $runext = 'r';
      $extext = '';
    } else {
      $logext = 'log';
      $diffext = 'diff';
      $baseext = 'base';
      $runext = 'run';
      $extext = '.';
    }
    $extext = '_' if $^O eq 'VMS';
    $log_filename = "$testpath.$logext";
    $diff_filename = "$testpath.$diffext";
    $base_filename = "$testpath.$baseext";
    $run_filename = "$testpath.$runext";
    $tmp_filename = "$testpath.$tmpfilesuffix";

    -f $perl_testname or die "Invalid test: $testname\n\n";

    setup_for_test();

    $output = "........................................................ ";

    substr($output,0,length($testname)) = "$testname ";

    print $output;

    $tests_run = 0;
    $tests_passed = 0;

    # Run the test!
    $code = do $perl_testname;

    ++$categories_run;
    $total_tests_run += $tests_run;
    $total_tests_passed += $tests_passed;

    # How did it go?
    if (!defined($code)) {
      # Failed to parse or called die
      if (length ($@)) {
        warn "\n*** Test died ($testname): $@\n";
      } else {
        warn "\n*** Couldn't parse $perl_testname\n";
      }
      $status = "FAILED ($tests_passed/$tests_run passed)";

    } elsif ($code == -1) {
      # Skipped... not supported
      $status = "N/A";
      --$categories_run;

    } elsif ($code != 1) {
      # Bad result... this shouldn't really happen.  Usually means that
      # the suite forgot to end with "1;".
      warn "\n*** Test returned $code\n";
      $status = "FAILED ($tests_passed/$tests_run passed)";

    } elsif ($tests_run == 0) {
      # Nothing was done!!
      $status = "FAILED (no tests found!)";

    } elsif ($tests_run > $tests_passed) {
      # Lose!
      $status = "FAILED ($tests_passed/$tests_run passed)";

    } else {
      # Win!
      ++$categories_passed;
      $status = "ok     ($tests_passed passed)";

      # Clean up
      for ($i = $num_of_tmpfiles; $i; $i--) {
        rmfiles($tmp_filename . num_suffix($i));
      }
      for ($i = $num_of_logfiles ? $num_of_logfiles : 1; $i; $i--) {
        rmfiles($log_filename . num_suffix($i));
        rmfiles($base_filename . num_suffix($i));
      }
    }

    # If the verbose option has been specified, then a short description
    # of each test is printed before displaying the results of each test
    # describing WHAT is being tested.

    if ($verbose) {
      if ($detail) {
        print "\nWHAT IS BEING TESTED\n";
        print "--------------------";
      }
      print "\n\n$description\n\n";
    }

    # If the detail option has been specified, then the details of HOW
    # the test is testing what it says it is testing in the verbose output
    # will be displayed here before the results of the test are displayed.

    if ($detail) {
      print "\nHOW IT IS TESTED\n";
      print "----------------";
      print "\n\n$details\n\n";
    }

    print "$status\n";
  }
}

# If the keep flag is not set, this subroutine deletes all filenames that
# are sent to it.

sub rmfiles
{
  my (@files) = @_;

  if (!$keep) {
    return (unlink @files);
  }

  return 1;
}

sub print_standard_usage
{
  my ($plname, @moreusage) = @_;

  print "usage:\t$plname [testname] [-verbose] [-detail] [-keep]\n";
  print "\t\t\t[-profile] [-usage] [-help] [-debug]\n";
  foreach (@moreusage) {
    print "\t\t\t$_\n";
  }
}

sub print_standard_help
{
  my (@morehelp) = @_;
  my $t = "      ";

  my $line = "Test Driver For $testee";
  print "$line\n";
  $line = "=" x length ($line);
  print "$line\n";

  print_usage();

  print "\ntestname\n"
      . "${t}You may, if you wish, run only ONE test if you know the name\n"
      . "${t}of that test and specify this name anywhere on the command\n"
      . "${t}line.  Otherwise ALL existing tests in the scripts directory\n"
      . "${t}will be run.\n"
      . "-verbose\n"
      . "${t}If this option is given, a description of every test is\n"
      . "${t}displayed before the test is run. (Not all tests may have\n"
      . "${t}descriptions at this time)\n"
      . "-detail\n"
      . "${t}If this option is given, a detailed description of every\n"
      . "${t}test is displayed before the test is run. (Not all tests\n"
      . "${t}have descriptions at this time)\n"
      . "-profile\n"
      . "${t}If this option is given, then the profile file\n"
      . "${t}is added to other profiles every time $testee is run.\n"
      . "${t}This option only works on VOS at this time.\n"
      . "-keep\n"
      . "${t}You may give this option if you DO NOT want ANY\n"
      . "${t}of the files generated by the tests to be deleted. \n"
      . "${t}Without this option, all files generated by the test will\n"
      . "${t}be deleted IF THE TEST PASSES.\n"
      . "-debug\n"
      . "${t}Use this option if you would like to see all of the system\n"
      . "${t}calls issued and their return status while running the tests\n"
      . "${t}This can be helpful if you're having a problem adding a test\n"
      . "${t}to the suite, or if the test fails!\n";

  foreach $line (@morehelp) {
    my $tline = $line;
    if (substr ($tline, 0, 1) eq "\t") {
      substr ($tline, 0, 1) = $t;
    }
    print "$tline\n";
  }
}

#######################################################################
###########         Generic Test Driver Subroutines         ###########
#######################################################################

sub get_caller
{
  my $depth = defined ($_[0]) ? $_[0] : 1;
  my ($pkg, $filename, $linenum) = caller ($depth + 1);
  return "$filename: $linenum";
}

sub error
{
  my $message = $_[0];
  my $caller = &get_caller (1);

  if (defined ($_[1])) {
    $caller = &get_caller ($_[1] + 1) . " -> $caller";
  }

  die "$caller: $message";
}

sub compare_output
{
  my ($answer,$logfile) = @_;
  my ($slurp, $answer_matched) = ('', 0);

  ++$tests_run;

  if (! defined $answer) {
    print "Ignoring output ........ " if $debug;
    $answer_matched = 1;
  } else {
    print "Comparing output ........ " if $debug;

    $slurp = &read_file_into_string ($logfile);

    # For make, get rid of any time skew error before comparing--too bad this
    # has to go into the "generic" driver code :-/
    $slurp =~ s/^.*modification time .*in the future.*\n//gm;
    $slurp =~ s/^.*Clock skew detected.*\n//gm;

    if ($slurp eq $answer) {
        $answer_matched = 1;
    } else {
      # See if it is a slash or CRLF problem
      my ($answer_mod, $slurp_mod) = ($answer, $slurp);

      $answer_mod =~ tr,\\,/,;
      $answer_mod =~ s,\r\n,\n,gs;

      $slurp_mod =~ tr,\\,/,;
      $slurp_mod =~ s,\r\n,\n,gs;

      $answer_matched = ($slurp_mod eq $answer_mod);
      if ($^O eq 'VMS') {

        # VMS has extra blank lines in output sometimes.
        # Ticket #41760
        if (!$answer_matched) {
          $slurp_mod =~ s/\n\n+/\n/gm;
          $slurp_mod =~ s/\A\n+//g;
          $answer_matched = ($slurp_mod eq $answer_mod);
        }

        # VMS adding a "Waiting for unfinished jobs..."
        # Remove it for now to see what else is going on.
        if (!$answer_matched) {
          $slurp_mod =~ s/^.+\*\*\* Waiting for unfinished jobs.+$//m;
          $slurp_mod =~ s/\n\n/\n/gm;
          $slurp_mod =~ s/^\n+//gm;
          $answer_matched = ($slurp_mod eq $answer_mod);
        }

        # VMS wants target device to exist or generates an error,
        # Some test tagets look like VMS devices and trip this.
        if (!$answer_matched) {
          $slurp_mod =~ s/^.+\: no such device or address.*$//gim;
          $slurp_mod =~ s/\n\n/\n/gm;
          $slurp_mod =~ s/^\n+//gm;
          $answer_matched = ($slurp_mod eq $answer_mod);
        }

        # VMS error message has a different case
        if (!$answer_matched) {
          $slurp_mod =~ s/no such file /No such file /gm;
          $answer_matched = ($slurp_mod eq $answer_mod);
        }

        # VMS is putting comas instead of spaces in output
        if (!$answer_matched) {
          $slurp_mod =~ s/,/ /gm;
          $answer_matched = ($slurp_mod eq $answer_mod);
        }

        # VMS Is sometimes adding extra leading spaces to output?
        if (!$answer_matched) {
           my $slurp_mod = $slurp_mod;
           $slurp_mod =~ s/^ +//gm;
           $answer_matched = ($slurp_mod eq $answer_mod);
        }

        # VMS port not handling POSIX encoded child status
        # Translate error case it for now.
        if (!$answer_matched) {
          $slurp_mod =~ s/0x1035a00a/1/gim;
          $answer_matched = 1 if $slurp_mod =~ /\Q$answer_mod\E/i;

        }
        if (!$answer_matched) {
          $slurp_mod =~ s/0x1035a012/2/gim;
          $answer_matched = ($slurp_mod eq $answer_mod);
        }

        # Tests are using a UNIX null command, temp hack
        # until this can be handled by the VMS port.
        # ticket # 41761
        if (!$answer_matched) {
          $slurp_mod =~ s/^.+DCL-W-NOCOMD.*$//gim;
          $slurp_mod =~ s/\n\n+/\n/gm;
          $slurp_mod =~ s/^\n+//gm;
          $answer_matched = ($slurp_mod eq $answer_mod);
        }
        # Tests are using exit 0;
        # this generates a warning that should stop the make, but does not
        if (!$answer_matched) {
          $slurp_mod =~ s/^.+NONAME-W-NOMSG.*$//gim;
          $slurp_mod =~ s/\n\n+/\n/gm;
          $slurp_mod =~ s/^\n+//gm;
          $answer_matched = ($slurp_mod eq $answer_mod);
        }

        # VMS is sometimes adding single quotes to output?
        if (!$answer_matched) {
          my $noq_slurp_mod = $slurp_mod;
          $noq_slurp_mod =~ s/\'//gm;
          $answer_matched = ($noq_slurp_mod eq $answer_mod);

          # And missing an extra space in output
          if (!$answer_matched) {
            $noq_answer_mod = $answer_mod;
            $noq_answer_mod =~ s/\h\h+/ /gm;
            $answer_matched = ($noq_slurp_mod eq $noq_answer_mod);
          }

          # VMS adding ; to end of some lines.
          if (!$answer_matched) {
            $noq_slurp_mod =~ s/;\n/\n/gm;
            $answer_matched = ($noq_slurp_mod eq $noq_answer_mod);
          }

          # VMS adding trailing space to end of some quoted lines.
          if (!$answer_matched) {
            $noq_slurp_mod =~ s/\h+\n/\n/gm;
            $answer_matched = ($noq_slurp_mod eq $noq_answer_mod);
          }

          # And VMS missing leading blank line
          if (!$answer_matched) {
            $noq_answer_mod =~ s/\A\n//g;
            $answer_matched = ($noq_slurp_mod eq $noq_answer_mod);
          }

          # Unix double quotes showing up as single quotes on VMS.
          if (!$answer_matched) {
            $noq_answer_mod =~ s/\"//g;
            $answer_matched = ($noq_slurp_mod eq $noq_answer_mod);
          }
        }
      }

      # If it still doesn't match, see if the answer might be a regex.
      if (!$answer_matched && $answer =~ m,^/(.+)/$,) {
        $answer_matched = ($slurp =~ /$1/);
        if (!$answer_matched && $answer_mod =~ m,^/(.+)/$,) {
            $answer_matched = ($slurp_mod =~ /$1/);
        }
      }
    }
  }

  if ($answer_matched && $test_passed) {
    print "ok\n" if $debug;
    ++$tests_passed;
    return 1;
  }

  if (! $answer_matched) {
    print "DIFFERENT OUTPUT\n" if $debug;

    &create_file (&get_basefile, $answer);
    &create_file (&get_runfile, $command_string);

    print "\nCreating Difference File ...\n" if $debug;

    # Create the difference file

    my $command = "diff -c " . &get_basefile . " " . $logfile;
    &run_command_with_output(&get_difffile,$command);
  }

  return 0;
}

sub read_file_into_string
{
  my ($filename) = @_;
  my $oldslash = $/;
  undef $/;

  open (RFISFILE, '<', $filename) or return "";
  my $slurp = <RFISFILE>;
  close (RFISFILE);

  $/ = $oldslash;

  return $slurp;
}

my @OUTSTACK = ();
my @ERRSTACK = ();

sub attach_default_output
{
  my ($filename) = @_;

  if ($vos)
  {
    my $code = system "++attach_default_output_hack $filename";
    $code == -2 or &error ("adoh death\n", 1);
    return 1;
  }

  my $dup = undef;
  open($dup, '>&', STDOUT) or error("ado: $! duping STDOUT\n", 1);
  push @OUTSTACK, $dup;

  $dup = undef;
  open($dup, '>&', STDERR) or error("ado: $! duping STDERR\n", 1);
  push @ERRSTACK, $dup;

  open(STDOUT, '>', $filename) or error("ado: $filename: $!\n", 1);
  open(STDERR, ">&STDOUT") or error("ado: $filename: $!\n", 1);
}

# close the current stdout/stderr, and restore the previous ones from
# the "stack."

sub detach_default_output
{
  if ($vos)
  {
    my $code = system "++detach_default_output_hack";
    $code == -2 or &error ("ddoh death\n", 1);
    return 1;
  }

  @OUTSTACK or error("default output stack has flown under!\n", 1);

  close(STDOUT);
  close(STDERR) unless $^O eq 'VMS';


  open (STDOUT, '>&', pop @OUTSTACK) or error("ddo: $! duping STDOUT\n", 1);
  open (STDERR, '>&', pop @ERRSTACK) or error("ddo: $! duping STDERR\n", 1);
}

sub _run_with_timeout
{
  my $code;
  if ($^O eq 'VMS') {
    #local $SIG{ALRM} = sub {
    #    my $e = $ERRSTACK[0];
    #    print $e "\nTest timed out after $test_timeout seconds\n";
    #    die "timeout\n";
    #};
    #alarm $test_timeout;
    system(@_);
    #alarm 0;
    my $severity = ${^CHILD_ERROR_NATIVE} & 7;
    $code = 0;
    if (($severity & 1) == 0) {
      $code = 512;
    }

    # Get the vms status.
    my $vms_code = ${^CHILD_ERROR_NATIVE};

    # Remove the print status bit
    $vms_code &= ~0x10000000;

    # Posix code translation.
    if (($vms_code & 0xFFFFF000) == 0x35a000) {
      $code = (($vms_code & 0xFFF) >> 3) * 256;
    }

  } elsif ($port_type eq 'W32') {
    my $pid = system(1, @_);
    $pid > 0 or die "Cannot execute $_[0]\n";
    local $SIG{ALRM} = sub {
      my $e = $ERRSTACK[0];
      print $e "\nTest timed out after $test_timeout seconds\n";
      kill -9, $pid;
      die "timeout\n";
    };
    alarm $test_timeout;
    my $r = waitpid($pid, 0);
    alarm 0;
    $r == -1 and die "No such pid: $pid\n";
    # This shouldn't happen since we wait forever or timeout via SIGALRM
    $r == 0 and die "No process exited.\n";
    $code = $?;

  } else {
    my $pid = fork();
    if (! $pid) {
      exec(@_) or die "exec: Cannot execute $_[0]: $!\n";
    }
    local $SIG{ALRM} = sub {
      my $e = $ERRSTACK[0];
      print $e "\nTest timed out after $test_timeout seconds\n";
      # Resend the alarm to our process group to kill the children.
      $SIG{ALRM} = 'IGNORE';
      kill -14, $$;
      die "timeout\n";
    };
    alarm $test_timeout;
    my $r = waitpid($pid, 0);
    alarm 0;
    $r == -1 and die "No such pid: $pid\n";
    # This shouldn't happen since we wait forever or timeout via SIGALRM
    $r == 0 and die "No process exited.\n";
    $code = $?;
  }

  return $code;
}

# This runs a command without any debugging info.
sub _run_command
{
  # We reset this before every invocation.  On Windows I think there is only
  # one environment, not one per process, so I think that variables set in
  # test scripts might leak into subsequent tests if this isn't reset--???
  resetENV();

  my $orig = $SIG{ALRM};
  my $code = eval { _run_with_timeout(@_); };
  $SIG{ALRM} = $orig;

  if ($@) {
    # The eval failed.  If it wasn't SIGALRM then die.
    $@ eq "timeout\n" or die "Command failed: $@";
    $code = 14;
  }

  return $code;
}

# run one command (passed as a list of arg 0 - n), returning 0 on success
# and nonzero on failure.

sub run_command
{
  print "\nrun_command: @_\n" if $debug;
  my $code = _run_command(@_);
  print "run_command returned $code.\n" if $debug;
  print "vms status = ${^CHILD_ERROR_NATIVE}\n" if $debug and $^O eq 'VMS';
  return $code;
}

# run one command (passed as a list of arg 0 - n, with arg 0 being the
# second arg to this routine), returning 0 on success and non-zero on failure.
# The first arg to this routine is a filename to connect to the stdout
# & stderr of the child process.

sub run_command_with_output
{
  my $filename = shift;

  print "\nrun_command_with_output($filename,$runname): @_\n" if $debug;
  &attach_default_output ($filename);
  my $code = eval { _run_command(@_) };
  my $err = $@;
  &detach_default_output;

  $err and die $err;

  print "run_command_with_output returned $code.\n" if $debug;
  print "vms status = ${^CHILD_ERROR_NATIVE}\n" if $debug and $^O eq 'VMS';
  return $code;
}

# performs the equivalent of an "rm -rf" on the first argument.  Like
# rm, if the path ends in /, leaves the (now empty) directory; otherwise
# deletes it, too.

sub remove_directory_tree
{
  my ($targetdir) = @_;
  my ($nuketop) = 1;

  my $ch = substr ($targetdir, length ($targetdir) - 1);
  if ($ch eq "/" || $ch eq $pathsep) {
    $targetdir = substr ($targetdir, 0, length ($targetdir) - 1);
    $nuketop = 0;
  }

  -e $targetdir or return 1;

  &remove_directory_tree_inner ("RDT00", $targetdir) or return 0;
  if ($nuketop && !rmdir ($targetdir)) {
    print "Cannot remove $targetdir: $!\n";
    return 0;
  }

  return 1;
}

sub remove_directory_tree_inner
{
  my ($dirhandle, $targetdir) = @_;

  opendir ($dirhandle, $targetdir) or return 0;
  my $subdirhandle = $dirhandle;
  $subdirhandle++;
  while (my $object = readdir ($dirhandle)) {
    $object =~ /^(\.\.?|CVS|RCS)$/ and next;
    $object = "$targetdir$pathsep$object";

    lstat ($object);
    if (-d _ && &remove_directory_tree_inner ($subdirhandle, $object)) {
      if (!rmdir($object)) {
        print "Cannot remove $object: $!\n";
        return 0;
      }
    } else {
      if ($^O ne 'VMS') {
        if (!unlink $object) {
          print "Cannot unlink $object: $!\n";
          return 0;
        }
      } else {
        # VMS can have multiple versions of a file.
        1 while unlink $object;
      }
    }
  }
  closedir ($dirhandle);
  return 1;
}

# We used to use this behavior for this function:
#
#sub touch
#{
#  my (@filenames) = @_;
#  my $now = time;
#
#  foreach my $file (@filenames) {
#    utime ($now, $now, $file)
#          or (open (TOUCHFD, ">> $file") and close (TOUCHFD))
#               or &error ("Couldn't touch $file: $!\n", 1);
#  }
#  return 1;
#}
#
# But this behaves badly on networked filesystems where the time is
# skewed, because it sets the time of the file based on the _local_
# host.  Normally when you modify a file, it's the _remote_ host that
# determines the modtime, based on _its_ clock.  So, instead, now we open
# the file and write something into it to force the remote host to set
# the modtime correctly according to its clock.
#

sub touch
{
  foreach my $file (@_) {
    (open(T, '>>', $file) and print(T "\n") and close(T))
        or &error("Couldn't touch $file: $!\n", 1);
  }

  return @_;
}

# Touch with a time offset.  To DTRT, call touch() then use stat() to get the
# access/mod time for each file and apply the offset.

sub utouch
{
  my $off = shift;

  &touch(@_);

  foreach my $f (@_) {
      my @s = stat($f);
      utime($s[8]+$off, $s[9]+$off, $f);
  }

  return @_;
}

# open a file, write some stuff to it, and close it.

sub create_file
{
  my ($filename, @lines) = @_;

  open (CF, "> $filename") or &error ("Couldn't open $filename: $!\n", 1);
  foreach $line (@lines) {
    print CF $line;
  }
  close (CF);
}

# create a directory tree described by an associative array, wherein each
# key is a relative pathname (using slashes) and its associated value is
# one of:
#    DIR            indicates a directory
#    FILE:contents  indicates a file, which should contain contents +\n
#    LINK:target    indicates a symlink, pointing to $basedir/target
# The first argument is the dir under which the structure will be created
# (the dir will be made and/or cleaned if necessary); the second argument
# is the associative array.

sub create_dir_tree
{
  my ($basedir, %dirtree) = @_;

  &remove_directory_tree ("$basedir");
  mkdir ($basedir, 0777) or &error ("Couldn't mkdir $basedir: $!\n", 1);

  foreach my $path (sort keys (%dirtree)) {
    if ($dirtree {$path} =~ /^DIR$/) {
      mkdir ("$basedir/$path", 0777)
          or &error ("Couldn't mkdir $basedir/$path: $!\n", 1);

    } elsif ($dirtree {$path} =~ /^FILE:(.*)$/) {
      &create_file ("$basedir/$path", $1 . "\n");

    } elsif ($dirtree {$path} =~ /^LINK:(.*)$/) {
      symlink ("$basedir/$1", "$basedir/$path")
          or &error ("Couldn't symlink $basedir/$path -> $basedir/$1: $!\n", 1);

    } else {
      &error ("Bogus dirtree type: \"$dirtree{$path}\"\n", 1);
    }
  }
  if ($just_setup_tree) {
    die "Tree is setup...\n";
  }
}

# compare a directory tree with an associative array in the format used
# by create_dir_tree, above.
# The first argument is the dir under which the structure should be found;
# the second argument is the associative array.

sub compare_dir_tree
{
  my ($basedir, %dirtree) = @_;
  my $bogus = 0;

  opendir (DIR, $basedir) or &error ("Couldn't open $basedir: $!\n", 1);
  my @allfiles = grep (!/^(\.\.?|CVS|RCS)$/, readdir (DIR) );
  closedir (DIR);
  if ($debug) {
    print "dirtree: (%dirtree)\n$basedir: (@allfiles)\n";
  }

  foreach my $path (sort keys (%dirtree))
  {
    if ($debug) {
      print "Checking $path ($dirtree{$path}).\n";
    }

    my $found = 0;
    foreach my $i (0 .. $#allfiles) {
      if ($allfiles[$i] eq $path) {
        splice (@allfiles, $i, 1);  # delete it
        if ($debug) {
          print "     Zapped $path; files now (@allfiles).\n";
        }
        lstat ("$basedir/$path");
        $found = 1;
        last;
      }
    }

    if (!$found) {
      print "compare_dir_tree: $path does not exist.\n";
      $bogus = 1;
      next;
    }

    if ($dirtree {$path} =~ /^DIR$/) {
      if (-d _ && opendir (DIR, "$basedir/$path") ) {
        my @files = readdir (DIR);
        closedir (DIR);
        @files = grep (!/^(\.\.?|CVS|RCS)$/ && ($_ = "$path/$_"), @files);
        push (@allfiles, @files);
        if ($debug)
        {
          print "     Read in $path; new files (@files).\n";
        }

      } else {
        print "compare_dir_tree: $path is not a dir.\n";
        $bogus = 1;
      }

    } elsif ($dirtree {$path} =~ /^FILE:(.*)$/) {
      if (-l _ || !-f _) {
        print "compare_dir_tree: $path is not a file.\n";
        $bogus = 1;
        next;
      }

      if ($1 ne "*") {
        my $contents = &read_file_into_string ("$basedir/$path");
        if ($contents ne "$1\n") {
          print "compare_dir_tree: $path contains wrong stuff."
              . "  Is:\n$contentsShould be:\n$1\n";
          $bogus = 1;
        }
      }

    } elsif ($dirtree {$path} =~ /^LINK:(.*)$/) {
      my $target = $1;
      if (!-l _) {
        print "compare_dir_tree: $path is not a link.\n";
        $bogus = 1;
        next;
      }

      my $contents = readlink ("$basedir/$path");
      $contents =~ tr/>/\//;
      my $fulltarget = "$basedir/$target";
      $fulltarget =~ tr/>/\//;
      if (!($contents =~ /$fulltarget$/)) {
        if ($debug) {
          $target = $fulltarget;
        }
        print "compare_dir_tree: $path should be link to $target, "
            . "not $contents.\n";
        $bogus = 1;
      }

    } else {
      &error ("Bogus dirtree type: \"$dirtree{$path}\"\n", 1);
    }
  }

  if ($debug) {
    print "leftovers: (@allfiles).\n";
  }

  foreach my $file (@allfiles) {
    print "compare_dir_tree: $file should not exist.\n";
    $bogus = 1;
  }

  return !$bogus;
}

# this subroutine generates the numeric suffix used to keep tmp filenames,
# log filenames, etc., unique.  If the number passed in is 1, then a null
# string is returned; otherwise, we return ".n", where n + 1 is the number
# we were given.

sub num_suffix
{
  my ($num) = @_;
  if (--$num > 0) {
    return "$extext$num";
  }

  return "";
}

# This subroutine returns a log filename with a number appended to
# the end corresponding to how many logfiles have been created in the
# current running test.  An optional parameter may be passed (0 or 1).
# If a 1 is passed, then it does NOT increment the logfile counter
# and returns the name of the latest logfile.  If either no parameter
# is passed at all or a 0 is passed, then the logfile counter is
# incremented and the new name is returned.

sub get_logfile
{
  my ($no_increment) = @_;

  $num_of_logfiles += !$no_increment;

  return ($log_filename . &num_suffix ($num_of_logfiles));
}

# This subroutine returns a base (answer) filename with a number
# appended to the end corresponding to how many logfiles (and thus
# base files) have been created in the current running test.
# NO PARAMETERS ARE PASSED TO THIS SUBROUTINE.

sub get_basefile
{
  return ($base_filename . &num_suffix ($num_of_logfiles));
}

# This subroutine returns a difference filename with a number appended
# to the end corresponding to how many logfiles (and thus diff files)
# have been created in the current running test.

sub get_difffile
{
  return ($diff_filename . &num_suffix ($num_of_logfiles));
}

# This subroutine returns a command filename with a number appended
# to the end corresponding to how many logfiles (and thus command files)
# have been created in the current running test.

sub get_runfile
{
  return ($run_filename . &num_suffix ($num_of_logfiles));
}

# just like logfile, only a generic tmp filename for use by the test.
# they are automatically cleaned up unless -keep was used, or the test fails.
# Pass an argument of 1 to return the same filename as the previous call.

sub get_tmpfile
{
  my ($no_increment) = @_;

  $num_of_tmpfiles += !$no_increment;

  return ($tmp_filename . &num_suffix ($num_of_tmpfiles));
}

1;
