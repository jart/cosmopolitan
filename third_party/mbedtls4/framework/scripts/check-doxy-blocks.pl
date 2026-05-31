#!/usr/bin/env perl

# Detect comment blocks that are likely meant to be doxygen blocks but aren't.
#
# More precisely, look for normal comment block containing '\'.
# Of course one could use doxygen warnings, eg with:
#   sed -e '/EXTRACT/s/YES/NO/' doxygen/mbedtls.doxyfile | doxygen -
# but that would warn about any undocumented item, while our goal is to find
# items that are documented, but not marked as such by mistake.
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

use warnings;
use strict;
use File::Basename;

# C/header files in the following directories will be checked
my @mbedtls_directories = qw(include/mbedtls library doxygen/input);
my @tf_psa_crypto_directories = qw(include/psa include/tf-psa-crypto
                                   include/mbedtls
                                   drivers/builtin/include/mbedtls
                                   drivers/builtin/src core dispatch
                                   doxygen/input extras platform utilities);

# very naive pattern to find directives:
# everything with a backslach except '\0' and backslash at EOL
my $doxy_re = qr/\\(?!0|\n)/;

# Return an error code to the environment if a potential error in the
# source code is found.
my $exit_code = 0;

sub check_file {
    my ($fname) = @_;
    open my $fh, '<', $fname or die "Failed to open '$fname': $!\n";

    # first line of the last normal comment block,
    # or 0 if not in a normal comment block
    my $block_start = 0;
    while (my $line = <$fh>) {
        $block_start = $.   if $line =~ m/\/\*(?![*!])/;
        $block_start = 0    if $line =~ m/\*\//;
        if ($block_start and $line =~ m/$doxy_re/) {
            print "$fname:$block_start: directive on line $.\n";
            $block_start = 0; # report only one directive per block
            $exit_code = 1;
        }
    }

    close $fh;
}

sub check_dir {
    my ($dirname) = @_;
    for my $file (<$dirname/*.[ch]>) {
        check_file($file);
    }
}

open my $project_file, "scripts/project_name.txt" or die "This script must be run from Mbed TLS or TF-PSA-Crypto root directory";
my $project = <$project_file>;
chomp($project);
my @directories;

if ($project eq "TF-PSA-Crypto") {
    @directories = @tf_psa_crypto_directories
} elsif ($project eq "Mbed TLS") {
    @directories = @mbedtls_directories
}
# Check that the script is being run from the project's root directory.
for my $dir (@directories) {
    check_dir($dir)
}

exit $exit_code;

__END__
