#!/usr/bin/env perl
# -*-perl-*-

# Test driver for the Make test suite

# Usage:  run_make_tests  [testname]
#                         [-debug]
#                         [-help]
#                         [-verbose]
#                         [-keep]
#                         [-make <make prog>]
#                        (and others)

# Copyright (C) 1992-2020 Free Software Foundation, Inc.
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

# Add the working directory to @INC and load the test driver
use FindBin;
use lib "$FindBin::Bin";

our $testsroot = $FindBin::Bin;

require "test_driver.pl";

use File::Spec;

use Cwd;
$cwdpath = cwd();
($cwdvol, $cwddir, $_) = File::Spec->splitpath($cwdpath, 1);

# Some target systems might not have the POSIX module...
$has_POSIX = eval { require "POSIX.pm" };

%FEATURES = ();

$valgrind = 0;              # invoke make with valgrind
$valgrind_args = '';
$memcheck_args = '--num-callers=15 --tool=memcheck --leak-check=full --suppressions=guile.supp';
$massif_args = '--num-callers=15 --tool=massif --alloc-fn=xmalloc --alloc-fn=xcalloc --alloc-fn=xrealloc --alloc-fn=xstrdup --alloc-fn=xstrndup';
$pure_log = undef;

# The location of the GNU make source directory
$srcdir = undef;
$fqsrcdir = undef;
$srcvol = undef;

# The location of the build directory
$blddir = undef;
$fqblddir = undef;
$bldvol = undef;

$make_path = undef;
@make_command = ();

$command_string = '';

$all_tests = 0;

# Shell commands

$sh_name = '/bin/sh';
$is_posix_sh = 1;

$CMD_rmfile = 'rm -f';

# rmdir broken in some Perls on VMS.
if ($^O eq 'VMS')
{
  require VMS::Filespec;
  VMS::Filespec->import();

  sub vms_rmdir {
    my $vms_file = vmspath($_[0]);
    $vms_file = fileify($vms_file);
    my $ret = unlink(vmsify($vms_file));
    return $ret
  };

  *CORE::GLOBAL::rmdir = \&vms_rmdir;

  $CMD_rmfile = 'delete_file -no_ask';
}

%CONFIG_FLAGS = ();

# Find the strings that will be generated for various error codes.
# We want them from the C locale regardless of our current locale.

$ERR_no_such_file = undef;
$ERR_read_only_file = undef;
$ERR_unreadable_file = undef;
$ERR_nonexe_file = undef;
$ERR_exe_dir = undef;

{
  use locale;

  my $loc = undef;
  if ($has_POSIX) {
      POSIX->import(qw(locale_h));
      # Windows has POSIX locale, but only LC_ALL not LC_MESSAGES
      $loc = POSIX::setlocale(&POSIX::LC_ALL);
      POSIX::setlocale(&POSIX::LC_ALL, 'C');
  }

  if (open(my $F, '<', 'file.none')) {
      print "Opened non-existent file! Skipping related tests.\n";
  } else {
      $ERR_no_such_file = "$!";
  }

  unlink('file.out');
  touch('file.out');

  chmod(0444, 'file.out');
  if (open(my $F, '>', 'file.out')) {
      print "Opened read-only file! Skipping related tests.\n";
      close($F);
  } else {
      $ERR_read_only_file = "$!";
  }

  $_ = `./file.out 2>/dev/null`;
  if ($? == 0) {
      print "Executed non-executable file!  Skipping related tests.\n";
  } else {
      $ERR_nonexe_file = "$!";
  }

  $_ = `./. 2>/dev/null`;
  if ($? == 0) {
      print "Executed directory!  Skipping related tests.\n";
  } else {
      $ERR_exe_dir = "$!";
  }

  chmod(0000, 'file.out');
  if (open(my $F, '<', 'file.out')) {
      print "Opened unreadable file!  Skipping related tests.\n";
      close($F);
  } else {
      $ERR_unreadable_file = "$!";
  }

  unlink('file.out') or die "Failed to delete file.out: $!\n";

  $loc and POSIX::setlocale(&POSIX::LC_ALL, $loc);
}

#$SIG{INT} = sub { print STDERR "Caught a signal!\n"; die @_; };

sub valid_option
{
   local($option) = @_;

   if ($option =~ /^-make([-_]?path)?$/i) {
       $make_path = shift @argv;
       if (!-f $make_path) {
           print "$option $make_path: Not found.\n";
           exit 0;
       }
       return 1;
   }

   if ($option =~ /^-srcdir$/i) {
       $srcdir = shift @argv;
       if (! -f File::Spec->catfile($srcdir, 'src', 'gnumake.h')) {
           print "$option $srcdir: Not a valid GNU make source directory.\n";
           exit 0;
       }
       return 1;
   }

   if ($option =~ /^-all([-_]?tests)?$/i) {
       $all_tests = 1;
       return 1;
   }

   if ($option =~ /^-(valgrind|memcheck)$/i) {
       $valgrind = 1;
       $valgrind_args = $memcheck_args;
       return 1;
   }

   if ($option =~ /^-massif$/i) {
       $valgrind = 1;
       $valgrind_args = $massif_args;
       return 1;
   }

# This doesn't work--it _should_!  Someone badly needs to fix this.
#
#   elsif ($option =~ /^-work([-_]?dir)?$/)
#   {
#      $workdir = shift @argv;
#      return 1;
#   }

   return 0;
}


# This is an "all-in-one" function.  Arguments are as follows:
#
#  [0] (string):  The makefile to be tested.  undef means use the last one.
#  [1] (string):  Arguments to pass to make.
#  [2] (string):  Answer we should get back.
#  [3] (integer): Exit code we expect.  A missing code means 0 (success)

$makefile = undef;
$old_makefile = undef;
$mkpath = undef;
$make_name = undef;
$helptool = undef;

sub subst_make_string
{
    local $_ = shift;
    $makefile and s/#MAKEFILE#/$makefile/g;
    s/#MAKEPATH#/$mkpath/g;
    s/#MAKE#/$make_name/g;
    s/#PERL#/$perl_name/g;
    s/#PWD#/$cwdpath/g;
    # If we're using a shell
    s/#HELPER#/$perl_name $helptool/g;
    return $_;
}

sub run_make_test
{
  local ($makestring, $options, $answer, $err_code, $timeout) = @_;
  my @call = caller;

  # If the user specified a makefile string, create a new makefile to contain
  # it.  If the first value is not defined, use the last one (if there is
  # one).

  if (! defined $makestring) {
    defined $old_makefile
      or die "run_make_test(undef) invoked before run_make_test('...')\n";
    $makefile = $old_makefile;
  } else {
    if (! defined($makefile)) {
      $makefile = &get_tmpfile();
    }

    # Make sure it ends in a newline and substitute any special tokens.
    $makestring && $makestring !~ /\n$/s and $makestring .= "\n";
    $makestring = subst_make_string($makestring);

    # Populate the makefile!
    open(MAKEFILE, "> $makefile") or die "Failed to open $makefile: $!\n";
    print MAKEFILE $makestring;
    close(MAKEFILE) or die "Failed to write $makefile: $!\n";
  }

  # Do the same processing on $answer as we did on $makestring.
  if (defined $answer) {
      $answer && $answer !~ /\n$/s and $answer .= "\n";
      $answer = subst_make_string($answer);
  }

  run_make_with_options($makefile, $options, &get_logfile(0),
                        $err_code, $timeout, @call);
  &compare_output($answer, &get_logfile(1));

  $old_makefile = $makefile;
  $makefile = undef;
}

sub add_options {
  my $cmd = shift;

  foreach (@_) {
    if (ref($cmd)) {
      push(@$cmd, ref($_) ? @$_ : $_);
    } else {
      $cmd .= ' '.(ref($_) ? "@$_" : $_);
    }
  }

  return $cmd;
}

sub create_command {
  return !$_[0] || ref($_[0]) ? [@make_command] : join(' ', @make_command);
}

# The old-fashioned way...
# $options can be a scalar (string) or a ref to an array of options
# If it's a scalar the entire argument is passed to system/exec etc. as
# a single string.  If it's a ref then the array is passed to system/exec.
# Using a ref should be preferred as it's more portable but all the older
# invocations use strings.
sub run_make_with_options {
  my ($filename,$options,$logname,$expected_code,$timeout,@call) = @_;
  @call = caller unless @call;
  my $code;
  my $command = create_command($options);

  $expected_code = 0 unless defined($expected_code);

  # Reset to reflect this one test.
  $test_passed = 1;

  if ($filename) {
    $command = add_options($command, '-f', $filename);
  }

  if ($options) {
    if (!ref($options) && $^O eq 'VMS') {
      # Try to make sure arguments are properly quoted.
      # This does not handle all cases.
      # We should convert the tests to use array refs not strings

      # VMS uses double quotes instead of single quotes.
      $options =~ s/\'/\"/g;

      # If the leading quote is inside non-whitespace, then the
      # quote must be doubled, because it will be enclosed in another
      # set of quotes.
      $options =~ s/(\S)(\".*\")/$1\"$2\"/g;

      # Options must be quoted to preserve case if not already quoted.
      $options =~ s/(\S+)/\"$1\"/g;

      # Special fixup for embedded quotes.
      $options =~ s/(\"\".+)\"(\s+)\"(.+\"\")/$1$2$3/g;

      $options =~ s/(\A)(?:\"\")(.+)(?:\"\")/$1\"$2\"/g;

      # Special fixup for misc/general4 test.
      $options =~ s/""\@echo" "cc""/\@echo cc"/;
      $options =~ s/"\@echo link"""/\@echo link"/;

      # Remove shell escapes expected to be removed by bash
      if ($options !~ /path=pre/) {
        $options =~ s/\\//g;
      }

      # special fixup for options/eval
      $options =~ s/"--eval=\$\(info" "eval/"--eval=\$\(info eval/;

      print ("Options fixup = -$options-\n") if $debug;
    }

    $command = add_options($command, $options);
  }

  my $cmdstr = ref($command) ? "'".join("' '", @$command)."'" : $command;

  if (@call) {
    $command_string = "#$call[1]:$call[2]\n$cmdstr\n";
  } else {
    $command_string = $cmdstr;
  }

  if ($valgrind) {
    print VALGRIND "\n\nExecuting: $cmdstr\n";
  }

  {
      my $old_timeout = $test_timeout;
      $timeout and $test_timeout = $timeout;

      # If valgrind is enabled, turn off the timeout check
      $valgrind and $test_timeout = 0;

      if (ref($command)) {
          $code = run_command_with_output($logname, @$command);
      } else {
          $code = run_command_with_output($logname, $command);
      }
      $test_timeout = $old_timeout;
  }

  # Check to see if we have Purify errors.  If so, keep the logfile.
  # For this to work you need to build with the Purify flag -exit-status=yes

  if ($pure_log && -f $pure_log) {
    if ($code & 0x7000) {
      $code &= ~0x7000;

      # If we have a purify log, save it
      $tn = $pure_testname . ($num_of_logfiles ? ".$num_of_logfiles" : "");
      print("Renaming purify log file to $tn\n") if $debug;
      rename($pure_log, "$tn") or die "Can't rename $pure_log to $tn: $!\n";
      ++$purify_errors;
    } else {
      unlink($pure_log);
    }
  }

  if ($code != $expected_code) {
    print "Error running @make_command (expected $expected_code; got $code): $cmdstr\n";
    $test_passed = 0;
    &create_file (&get_runfile, $command_string);
    # If it's a SIGINT, stop here
    if ($code & 127) {
      print STDERR "\nCaught signal ".($code & 127)."!\n";
      ($code & 127) == 2 and exit($code);
    }
    return 0;
  }

  if ($profile & $vos) {
    system "add_profile @make_command";
  }

  return 1;
}

sub print_usage
{
   &print_standard_usage ("run_make_tests",
                          "[-make MAKE_PATHNAME] [-srcdir SRCDIR] [-memcheck] [-massif]",);
}

sub print_help
{
   &print_standard_help (
        "-make",
        "\tYou may specify the pathname of the copy of make to run.",
        "-srcdir",
        "\tSpecify the make source directory.",
        "-valgrind",
        "-memcheck",
        "\tRun the test suite under valgrind's memcheck tool.",
        "\tChange the default valgrind args with the VALGRIND_ARGS env var.",
        "-massif",
        "\tRun the test suite under valgrind's massif tool.",
        "\tChange the default valgrind args with the VALGRIND_ARGS env var."
       );
}

sub set_defaults
{
  # $profile = 1;
  $testee = "GNU make";
  $make_path = "make";
  $tmpfilesuffix = "mk";
  if ($port_type eq 'UNIX') {
    $scriptsuffix = '.sh';
  } elsif ($port_type eq 'VMS') {
    $scriptsuffix = '.com';
  } else {
    $scriptsuffix = '.bat';
  }
}

# This is no longer used: we import config-flags.pm instead
# sub parse_status
# {
#   if (open(my $fh, '<', "$_[0]/config.status")) {
#     while (my $line = <$fh>) {
#       $line =~ m/^[SD]\["([^\"]+)"\]=" *(.*)"/ and $CONFIG_FLAGS{$1} = $2;
#     }
#     return 1;
#   }
#   return 0;
# }

sub find_prog
{
  my $prog = $_[0];
  my ($v, $d, $f) = File::Spec->splitpath($prog);

  # If there's no directory then we need to search the PATH
  if (! $d) {
    foreach my $e (File::Spec->path()) {
      $prog = File::Spec->catfile($e, $f);
      if (-x $prog) {
        ($v, $d, $f) = File::Spec->splitpath($prog);
        last;
      }
    }
  }

  return ($v, $d, $f);
}

sub get_config
{
  return exists($CONFIG_FLAGS{$_[0]}) ? $CONFIG_FLAGS{$_[0]} : '';
}

sub set_more_defaults
{
  my $string;

  # Now that we have located make_path, locate the srcdir and blddir
  my ($mpv, $mpd, $mpf) = find_prog($make_path);

  # We have a make program so try to compute the blddir.
  if ($mpd) {
    my $f = File::Spec->catpath($mpv, File::Spec->catdir($mpd, 'tests'), 'config-flags.pm');
    if (-f $f) {
      $bldvol = $mpv;
      $blddir = $mpd;
    }
  }

  # If srcdir wasn't provided on the command line, try to find it.
  if (! $srcdir && $blddir) {
    # See if the blddir is the srcdir
    my $f = File::Spec->catpath($bldvol, File::Spec->catdir($blddir, 'src'), 'gnumake.h');
    if (-f $f) {
      $srcdir = $blddir;
      $srcvol = $bldvol;
    }
  }

  if (! $srcdir) {
    # Not found, see if our parent is the source dir
    my $f = File::Spec->catpath($cwdvol, File::Spec->catdir(File::Spec->updir(), 'src'), 'gnumake.h');
    if (-f $f) {
      $srcdir = File::Spec->updir();
      $srcvol = $cwdvol;
    }
  }

  # If we have srcdir but not blddir, set them equal
  if ($srcdir && !$blddir) {
    $blddir = $srcdir;
    $bldvol = $srcvol;
  }

  # Load the config flags
  if (!$blddir) {
    warn "Cannot locate config-flags.pm (no blddir)\n";
  } else {
    my $f = File::Spec->catpath($bldvol, File::Spec->catdir($blddir, 'tests'), 'config-flags.pm');
    if (! -f $f) {
      warn "Cannot locate $f\n";
    } else {
      unshift(@INC, File::Spec->catpath($bldvol, File::Spec->catdir($blddir, 'tests'), ''));
      require "config-flags.pm";
    }
  }

  # Find the full pathname of Make.  For DOS systems this is more
  # complicated, so we ask make itself.
  if ($osname eq 'VMS') {
    $port_type = 'VMS-DCL' unless defined $ENV{"SHELL"};
    # On VMS pre-setup make to be found with simply 'make'.
    $make_path = 'make';
  } else {
    create_file('make.mk', 'all:;$(info $(MAKE))');
    my $mk = `$make_path -sf make.mk`;
    unlink('make.mk');
    chop $mk;
    $mk or die "FATAL ERROR: Cannot determine the value of \$(MAKE)\n";
    $make_path = $mk;
  }
  ($mpv, $mpd, $mpf) = File::Spec->splitpath($make_path);

  # Ask make what shell to use
  create_file('shell.mk', 'all:;$(info $(SHELL))');
  $sh_name = `$make_path -sf shell.mk`;
  unlink('shell.mk');
  chop $sh_name;
  if (! $sh_name) {
      print "Cannot determine shell\n";
      $is_posix_sh = 0;
  } else {
      my $o = `$sh_name -c ': do nothing' 2>&1`;
      $is_posix_sh = $? == 0 && $o eq '';
  }

  $string = `$make_path -v`;
  $string =~ /^(GNU Make [^,\n]*)/ or die "$make_path is not GNU make.  Version:\n$string";
  $testee_version = "$1\n";

  create_file('null.mk', '');

  my $redir = '2>&1';
  $redir = '' if os_name eq 'VMS';
  $string = `$make_path -f null.mk $redir`;
  if ($string =~ /(.*): \*\*\* No targets\.  Stop\./) {
    $make_name = $1;
  } else {
    $make_name = $mpf;
  }

  # prepend pwd if this is a relative path (ie, does not
  # start with a slash, but contains one).  Thanks for the
  # clue, Roland.

  if ($mpd && !File::Spec->file_name_is_absolute($make_path) && $cwdvol == $mpv) {
     $mkpath = File::Spec->catpath($cwdvol, File::Spec->catdir($cwd, $mpd), $mpf);
  } else {
     $mkpath = $make_path;
  }

  # Not with the make program, so see if we can get it out of the makefile
  if (! $srcdir && open(MF, '<', File::Spec->catfile(File::Spec->updir(), 'Makefile'))) {
    local $/ = undef;
    $_ = <MF>;
    close(MF);
    /^abs_srcdir\s*=\s*(.*?)\s*$/m;
    -f File::Spec->catfile($1, 'src', 'gnumake.h') and $srcdir = $1;
  }

  # At this point we should have srcdir and blddir: get fq versions
  $fqsrcdir = File::Spec->rel2abs($srcdir);
  $fqblddir = File::Spec->rel2abs($blddir);

  # Find the helper tool
  $helptool = File::Spec->catfile($fqsrcdir, 'tests', 'thelp.pl');

  # It's difficult to quote this properly in all the places it's used so
  # ensure it doesn't need to be quoted.
  $helptool =~ s,\\,/,g if $port_type eq 'W32';
  $helptool =~ s, ,\\ ,g;

  # Get Purify log info--if any.

  if (exists $ENV{PURIFYOPTIONS}
      && $ENV{PURIFYOPTIONS} =~ /.*-logfile=([^ ]+)/) {
    $pure_log = $1 || '';
    $pure_log =~ s/%v/$make_name/;
    $purify_errors = 0;
  }

  $string = `$make_path -j 2 -f null.mk $redir`;
  if ($string =~ /not supported/) {
    $parallel_jobs = 0;
  }
  else {
    $parallel_jobs = 1;
  }

  unlink('null.mk');

  create_file('features.mk', 'all:;$(info $(.FEATURES))');
  %FEATURES = map { $_ => 1 } split /\s+/, `$make_path -sf features.mk`;
  unlink('features.mk');

  # Set up for valgrind, if requested.

  @make_command = ($make_path);

  if ($valgrind) {
    my $args = $valgrind_args;
    open(VALGRIND, "> valgrind.out") or die "Cannot open valgrind.out: $!\n";
    #  -q --leak-check=yes
    exists $ENV{VALGRIND_ARGS} and $args = $ENV{VALGRIND_ARGS};
    @make_command = ('valgrind', '--log-fd='.fileno(VALGRIND));
    push(@make_command, split(' ', $args));
    push(@make_command, $make_path);
    # F_SETFD is 2
    fcntl(VALGRIND, 2, 0) or die "fcntl(setfd) failed: $!\n";
    system("echo Starting on `date` 1>&".fileno(VALGRIND));
    print "Enabled valgrind support.\n";
  }

  if ($debug) {
    print "Port type:    $port_type\n";
    print "Make command: @make_command\n";
    print "Shell path:   $sh_name".($is_posix_sh ? ' (POSIX)' : '')."\n";
    print "#PWD#:        $cwdpath\n";
    print "#PERL#:       $perl_name\n";
    print "#MAKEPATH#:   $mkpath\n";
    print "#MAKE#:       $make_name\n";
  }
}

sub setup_for_test
{
  $makefile = &get_tmpfile;
  if (-f $makefile) {
    unlink $makefile;
  }

  # Get rid of any Purify logs.
  if ($pure_log) {
    ($pure_testname = $testname) =~ tr,/,_,;
    $pure_testname = "$pure_log.$pure_testname";
    system("rm -f $pure_testname*");
    print("Purify testfiles are: $pure_testname*\n") if $debug;
  }
}

exit !&toplevel;
