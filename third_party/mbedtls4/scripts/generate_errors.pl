#!/usr/bin/env perl

# Generate error.c
#
# Usage: ./generate_errors.pl or scripts/generate_errors.pl without arguments,
# or generate_errors.pl crypto_include_dir tls_include_dir data_dir error_file
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

use strict;
use warnings;

my ($crypto_include_dir, $tls_include_dir, $data_dir, $error_file);

if( @ARGV ) {
    die "Invalid number of arguments" if scalar @ARGV != 4;
    ($crypto_include_dir, $tls_include_dir, $data_dir, $error_file) = @ARGV;

    -d $crypto_include_dir or die "No such directory: $crypto_include_dir\n";
    -d $tls_include_dir or die "No such directory: $tls_include_dir\n";
    -d $data_dir or die "No such directory: $data_dir\n";
} else {
    $crypto_include_dir = 'tf-psa-crypto/drivers/builtin/include/mbedtls';
    $tls_include_dir = 'include/mbedtls';
    $data_dir = 'scripts/data_files';
    $error_file = 'library/error.c';

    unless( -d $crypto_include_dir && -d $tls_include_dir && -d $data_dir ) {
        chdir '..' or die;
        -d $crypto_include_dir && -d $tls_include_dir && -d $data_dir
            or die "Without arguments, must be run from root or scripts\n"
    }
}

my $error_format_file = $data_dir.'/error.fmt';

my @low_level_modules = qw( AES ARIA ASN1 BASE64 BIGNUM
                            CAMELLIA CCM CHACHA20 CHACHAPOLY CMAC CTR_DRBG
                            ENTROPY ERROR GCM HKDF HMAC_DRBG LMS MD5
                            NET PBKDF2 PLATFORM POLY1305 RIPEMD160
                            SHA1 SHA256 SHA512 SHA3 THREADING );
my @high_level_modules = qw( CIPHER ECP MD
                             PEM PK PKCS12 PKCS5
                             RSA SSL X509 PKCS7 );

undef $/;

open(FORMAT_FILE, '<:crlf', "$error_format_file") or die "Opening error format file '$error_format_file': $!";
my $error_format = <FORMAT_FILE>;
close(FORMAT_FILE);

my @files = glob qq("$crypto_include_dir/*.h");
push(@files, glob qq("$tls_include_dir/*.h"));

push(@files, glob qq("$crypto_include_dir/private/*.h"));
push(@files, glob qq("$tls_include_dir/private/*.h"));

my @necessary_include_files;
my @matches;
foreach my $file (@files) {
    open(FILE, '<:crlf', $file) or die("$0: $file: $!");
    my $content = <FILE>;
    close FILE;
    my $found = 0;
    while ($content =~ m[
            # Both the before-comment and the after-comment are optional.
            # Only the comment content is a regex capture group. The comment
            # start and end parts are outside the capture group.
            (?:/\*[*!](?!<)             # Doxygen before-comment start
                ((?:[^*]|\*+[^*/])*)    # $1: Comment content (no */ inside)
                \*/)?                   # Comment end
            \s*\#\s*define\s+(MBEDTLS_ERR_\w+)  # $2: name
            \s+\-(0[Xx][0-9A-Fa-f]+)\s*         # $3: value (without the sign)
            (?:/\*[*!]<                 # Doxygen after-comment start
                ((?:[^*]|\*+[^*/])*)    # $4: Comment content (no */ inside)
                \*/)?                   # Comment end
    ]gsx) {
        my ($before, $name, $value, $after) = ($1, $2, $3, $4);
        # Discard Doxygen comments that are coincidentally present before
        # an error definition but not attached to it. This is ad hoc, based
        # on what actually matters (or mattered at some point).
        undef $before if defined($before) && $before =~ /\s*\\name\s/s;
        die "Description neither before nor after $name in $file\n"
          if !defined($before) && !defined($after);
        die "Description both before and after $name in $file\n"
          if defined($before) && defined($after);
        my $description = (defined($before) ? $before : $after);
        $description =~ s/^\s+//;
        $description =~ s/\n( *\*)? */ /g;
        $description =~ s/\.?\s+$//;
        push @matches, [$name, $value, $description, scalar($file =~ /^.*private\/[^\/]+$/)];
        ++$found;
    }
    if ($found) {
        my $include_name = $file;
        $include_name =~ s!.*/!!;
        $include_name = "error.h" if ($include_name eq "error_common.h");
        push @necessary_include_files, $include_name;
    }
}

my @ll_old_define = ("", "", "");
my @hl_old_define = ("", "", "");

my $ll_code_check = "";
my $hl_code_check = "";

my $headers = "";
my %included_headers;

my %error_codes_seen;

foreach my $match (@matches)
{
    my ($error_name, $error_code, $description, $is_private_header) = @$match;

    die "Duplicated error code: $error_code ($error_name)\n"
        if( $error_codes_seen{$error_code}++ );

    $description =~ s/\\/\\\\/g;

    my ($module_name) = $error_name =~ /^MBEDTLS_ERR_([^_]+)/;

    # Fix faulty ones
    $module_name = "BIGNUM" if ($module_name eq "MPI");
    $module_name = "CTR_DRBG" if ($module_name eq "CTR");
    $module_name = "HMAC_DRBG" if ($module_name eq "HMAC");

    my $define_name = $module_name;
    $define_name = "X509_USE,X509_CREATE" if ($define_name eq "X509");
    $define_name = "ASN1_PARSE" if ($define_name eq "ASN1");
    $define_name = "SSL_TLS" if ($define_name eq "SSL");
    $define_name = "PEM_PARSE,PEM_WRITE" if ($define_name eq "PEM");
    $define_name = "PKCS7" if ($define_name eq "PKCS7");
    $define_name = "ALG_SHA3_224,ALG_SHA3_256,ALG_SHA3_384,ALG_SHA3_512"
                   if ($define_name eq "SHA3");

    my $define_prefix = "MBEDTLS_";
    $define_prefix = "PSA_WANT_" if ($module_name eq "SHA3");

    my $define_suffix = "_C";
    $define_suffix = "" if ($module_name eq "SHA3");

    my $include_name = $module_name;
    $include_name =~ tr/A-Z/a-z/;

    # Fix faulty ones
    $include_name = "net_sockets" if ($module_name eq "NET");

    $included_headers{"${include_name}.h"} = $module_name;

    my $found_ll = grep $_ eq $module_name, @low_level_modules;
    my $found_hl = grep $_ eq $module_name, @high_level_modules;
    if (!$found_ll && !$found_hl)
    {
        printf("Error: Do not know how to handle: $module_name\n");
        exit 1;
    }

    my $code_check;
    my $old_define;
    my $white_space;
    my $first;

    if ($found_ll)
    {
        $code_check = \$ll_code_check;
        $old_define = \@ll_old_define;
        $white_space = '        ';
    }
    else
    {
        $code_check = \$hl_code_check;
        $old_define = \@hl_old_define;
        $white_space = '        ';
    }

    my $old_define_name   = \${$old_define}[0];
    my $old_define_prefix = \${$old_define}[1];
    my $old_define_suffix = \${$old_define}[2];

    if ($define_name ne ${$old_define_name})
    {
        if (${$old_define_name} ne "")
        {
            ${$code_check} .= "#endif /* ";
            $first = 0;
            foreach my $dep (split(/,/, ${$old_define_name}))
            {
                ${$code_check} .= " || \n          " if ($first++);
                ${$code_check} .= "${$old_define_prefix}${dep}${$old_define_suffix}";
            }
            ${$code_check} .= " */\n\n";
        }

        ${$code_check} .= "#if ";
        $headers .= "#if " if ($include_name ne "");
        $first = 0;
        foreach my $dep (split(/,/, ${define_name}))
        {
            ${$code_check} .= " || \\\n    " if ($first);
            $headers       .= " || \\\n    " if ($first++);

            ${$code_check} .= "defined(${define_prefix}${dep}${define_suffix})";
            $headers       .= "defined(${define_prefix}${dep}${define_suffix})"
                              if ($include_name ne "");
        }
        ${$code_check} .= "\n";

        if ($is_private_header) {
            $include_name = "private/" . $include_name;
        }

        $headers .= "\n#include \"mbedtls/${include_name}.h\"\n".
                    "#endif\n\n" if ($include_name ne "");
        ${$old_define_name}   = $define_name;
        ${$old_define_prefix} = $define_prefix;
        ${$old_define_suffix} = $define_suffix;
    }

    ${$code_check} .= "${white_space}case -($error_name):\n".
                      "${white_space}    return( \"$module_name - $description\" );\n"
};

if ($ll_old_define[0] ne "")
{
    $ll_code_check .= "#endif /* ";
    my $first = 0;
    foreach my $dep (split(/,/, $ll_old_define[0]))
    {
        $ll_code_check .= " || \n          " if ($first++);
        $ll_code_check .= "${ll_old_define[1]}${dep}${ll_old_define[2]}";
    }
    $ll_code_check .= " */\n";
}
if ($hl_old_define[0] ne "")
{
    $hl_code_check .= "#endif /* ";
    my $first = 0;
    foreach my $dep (split(/,/, $hl_old_define[0]))
    {
        $hl_code_check .= " || \n          " if ($first++);
        $hl_code_check .= "${hl_old_define[1]}${dep}${hl_old_define[2]}";
    }
    $hl_code_check .= " */\n";
}

$error_format =~ s/HEADER_INCLUDED\n/$headers/g;
$error_format =~ s/ *LOW_LEVEL_CODE_CHECKS\n/$ll_code_check/g;
$error_format =~ s/ *HIGH_LEVEL_CODE_CHECKS\n/$hl_code_check/g;

open(ERROR_FILE, ">$error_file") or die "Opening destination file '$error_file': $!";
print ERROR_FILE $error_format;
close(ERROR_FILE);

my $errors = 0;
for my $include_name (@necessary_include_files)
{
    if (not $included_headers{$include_name})
    {
        print STDERR "The header file \"$include_name\" defines error codes but has not been included!\n";
        ++$errors;
    }
}

exit !!$errors;
