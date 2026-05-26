#! /usr/bin/env perl

# Generate query_config.c
#
# The file query_config.c contains a C function that can be used to check if
# a configuration macro is defined and to retrieve its expansion in string
# form (if any). This facilitates querying the compile time configuration of
# the library, for example, for testing.
#
# The query_config.c is generated from the default configuration files
# include/mbedtls/mbedtls_config.h and include/psa/crypto_config.h.
# The idea is that mbedtls_config.h and crypto_config.h contain ALL the
# compile time configurations available in Mbed TLS (commented or uncommented).
# This script extracts the configuration macros from the two files and this
# information is used to automatically generate the body of the query_config()
# function by using the template in scripts/data_files/query_config.fmt.
#
# Usage: scripts/generate_query_config.pl without arguments, or
# generate_query_config.pl mbedtls_config_file psa_crypto_config_file template_file output_file
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

use strict;

my ($mbedtls_config_file, $psa_crypto_config_file, $query_config_format_file, $query_config_file);

my $default_mbedtls_config_file = "./include/mbedtls/mbedtls_config.h";
my $default_psa_crypto_config_file = "./tf-psa-crypto/include/psa/crypto_config.h";
my $default_query_config_format_file = "./scripts/data_files/query_config.fmt";
my $default_query_config_file = "./programs/test/query_config.c";

if( @ARGV ) {
    die "Invalid number of arguments - usage: $0 [MBED_TLS_CONFIG_FILE PSA_CRYPTO_CONFIG_FILE TEMPLATE_FILE OUTPUT_FILE]" if scalar @ARGV != 4;
    ($mbedtls_config_file, $psa_crypto_config_file, $query_config_format_file, $query_config_file) = @ARGV;

    -f $mbedtls_config_file or die "No such file: $mbedtls_config_file";
    -f $psa_crypto_config_file or die "No such file: $psa_crypto_config_file";
    -f $query_config_format_file or die "No such file: $query_config_format_file";
} else {
    $mbedtls_config_file = $default_mbedtls_config_file;
    $psa_crypto_config_file = $default_psa_crypto_config_file;
    $query_config_format_file = $default_query_config_format_file;
    $query_config_file = $default_query_config_file;

    unless(-f $mbedtls_config_file && -f $query_config_format_file  && -f $psa_crypto_config_file) {
        chdir '..' or die;
        -f $mbedtls_config_file && -f $query_config_format_file && -f $psa_crypto_config_file
          or die "No arguments supplied, must be run from project root or a first-level subdirectory\n";
    }
}
-f 'include/mbedtls/build_info.h'
  or die "$0: must be run from project root, or from a first-level subdirectory with no arguments\n";

# Excluded macros from the generated query_config.c. For example, macros that
# have commas or function-like macros cannot be transformed into strings easily
# using the preprocessor, so they should be excluded or the preprocessor will
# throw errors.
my @excluded = qw(
MBEDTLS_SSL_CIPHERSUITES
);
my $excluded_re = join '|', @excluded;

# This variable will contain the string to replace in the CHECK_CONFIG of the
# format file
my $config_check = "";
my $list_config = "";

for my $config_file ($mbedtls_config_file, $psa_crypto_config_file) {

    next unless defined($config_file);  # we might not have been given a PSA crypto config file

    open(CONFIG_FILE, "<", $config_file) or die "Opening config file '$config_file': $!";

    while (my $line = <CONFIG_FILE>) {
        if ($line =~ /^(\/\/)?\s*#\s*define\s+(MBEDTLS_\w+|PSA_WANT_\w+).*/) {
            my $name = $2;

            # Skip over the macro if it is in the excluded list
            next if $name =~ /$excluded_re/;

            $config_check .= <<EOT;
#if defined($name)
    if( strcmp( "$name", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( $name );
        return( 0 );
    }
#endif /* $name */

EOT

            $list_config .= <<EOT;
#if defined($name)
    OUTPUT_MACRO_NAME_VALUE($name);
#endif /* $name */

EOT
        }
    }

    close(CONFIG_FILE);
}

# We need to include all the headers with public APIs in case they
# define a macro to its default value when that configuration is not
# set in a header included by build_info.h (crypto_config.h,
# mbedtls_config.h, *adjust*.h). Some module-specific macros are set
# in that module's header. For simplicity, include all headers, with
# some ad hoc knowledge of headers that are included by other headers
# and should not be included directly. We don't include internal headers
# because those should not define configurable macros.
my @header_files = ();
my @header_roots = qw(
                         include
                         tf-psa-crypto/include
                         tf-psa-crypto/drivers/builtin/include
                    );
for my $root (@header_roots) {
    my @paths = glob "$root/*/*.h $root/*/*/*.h";
    map {s!^\Q$root/!!} @paths;
    # Exclude some headers that are included by build_info.h and cannot
    # be included directly.
    push @header_files, grep {!m[
            ^psa/crypto_(platform|struct)\.h$ | # have alt versions, included by psa/crypto.h anyway
            ^mbedtls/platform_time\.h$ | # errors without time.h
            _config\.h |
            [/_]adjust[/_]
        ]x} @paths;
}
my $include_headers = join('', map {"#include <$_>\n"} @header_files);

# Read the full format file into a string
local $/;
open(FORMAT_FILE, "<", $query_config_format_file) or die "Opening query config format file '$query_config_format_file': $!";
my $query_config_format = <FORMAT_FILE>;
close(FORMAT_FILE);

# Replace the body of the query_config() function with the code we just wrote
$query_config_format =~ s/INCLUDE_HEADERS/$include_headers/g;
$query_config_format =~ s/CHECK_CONFIG/$config_check/g;
$query_config_format =~ s/LIST_CONFIG/$list_config/g;

# Rewrite the query_config.c file
open(QUERY_CONFIG_FILE, ">", $query_config_file) or die "Opening destination file '$query_config_file': $!";
print QUERY_CONFIG_FILE $query_config_format;
close(QUERY_CONFIG_FILE);
