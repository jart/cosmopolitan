#!/usr/bin/env perl
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may
# not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

use strict;
use warnings;

if (!@ARGV || $ARGV[0] == '--help') {
    print <<EOF;
Usage: $0 mbedtls_test_foo <file.pem
       $0 TEST_FOO mbedtls_test_foo <file.pem
Print out a PEM file as C code defining a string constant.

Used to include some of the test data in /library/certs.c for
self-tests and sample programs.
EOF
    exit;
}

my $pp_name = @ARGV > 1 ? shift @ARGV : undef;
my $name = shift @ARGV;

my @lines = map {chomp; s/([\\"])/\\$1/g; "\"$_\\r\\n\""} <STDIN>;

if (defined $pp_name) {
    foreach ("#define $pp_name", @lines[0..@lines-2]) {
        printf "%-72s\\\n", $_;
    }
    print "$lines[@lines-1]\n";
    print "const char $name\[\] = $pp_name;\n";
} else {
    print "const char $name\[\] =";
    foreach (@lines) {
        print "\n$_";
    }
    print ";\n";
}
