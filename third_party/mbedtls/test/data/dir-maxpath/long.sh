#!/bin/sh
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

set -eu

: ${OPENSSL:=openssl}
NB=20

OPT="-days 3653 -sha256"

# generate self-signed root
$OPENSSL ecparam -name prime256v1 -genkey -out 00.key
$OPENSSL req -new -x509 -subj "/C=UK/O=mbed TLS/CN=CA00" $OPT \
             -key 00.key -out 00.crt

# cXX.pem is the chain starting at XX
cp 00.crt c00.pem

# generate long chain
i=1
while [ $i -le $NB ]; do
    UP=$( printf "%02d" $((i-1)) )
    ME=$( printf "%02d" $i )

    $OPENSSL ecparam -name prime256v1 -genkey -out ${ME}.key
    $OPENSSL req -new -subj "/C=UK/O=mbed TLS/CN=CA${ME}" \
                 -key ${ME}.key -out ${ME}.csr
    $OPENSSL x509 -req -CA ${UP}.crt -CAkey ${UP}.key -set_serial 1 $OPT \
                  -extfile int.opensslconf -extensions int \
                  -in ${ME}.csr -out ${ME}.crt

    cat ${ME}.crt c${UP}.pem > c${ME}.pem

    rm ${ME}.csr
    i=$((i+1))
done
