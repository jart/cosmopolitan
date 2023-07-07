#!/bin/sh
m=zero
make -j16 m=$m o/$m/tool/curl/curl.com || exit
for t in expired \
         wrong.host \
         self-signed \
         untrusted-root \
         revoked \
         dh480 \
         dh512 \
         no-common-name \
         no-subject \
         incomplete-chain \
         superfish \
         captive-portal \
         edellroot \
         mitm-software \
         rc4; do
  u=https://$t.badssl.com/
  echo
  echo $u
  o/$m/tool/curl/curl.com $u
done
