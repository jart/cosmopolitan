#!/usr/bin/env bash
set -ex
rm -rf o/samples
make -j16 o//third_party/zstd/zstd.com
o//third_party/zstd/zstd.com --maxdict=524288 -9 -T0 -B4096 --train $(find o//{examples,tool,third_party} -name \*.com)
o//third_party/zstd/zstd.com -9 --ultra -fz o//tool/curl/curl.com
o//third_party/zstd/zstd.com -D dictionary -9 --ultra -fz o//tool/curl/curl.com
o//third_party/zstd/zstd.com -9 --ultra -fz o//examples/hello.com
o//third_party/zstd/zstd.com -D dictionary -9 --ultra -fz o//examples/hello.com
o//third_party/zstd/zstd.com -9 --ultra -fz o//examples/hello2.com
o//third_party/zstd/zstd.com -D dictionary -9 --ultra -fz o//examples/hello2.com
o//third_party/zstd/zstd.com -9 --ultra -fz o//examples/hello3.com
o//third_party/zstd/zstd.com -D dictionary -9 --ultra -fz o//examples/hello3.com
