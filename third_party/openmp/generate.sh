#!/bin/sh

perl ~/vendor/llvm-project/openmp/runtime/tools/message-converter.pl \
  --os=lin --prefix=kmp_i18n --enum=third_party/openmp/kmp_i18n_id.inc \
  ~/vendor/llvm-project/openmp/runtime/src/i18n/en_US.txt || exit

perl ~/vendor/llvm-project/openmp/runtime/tools/message-converter.pl \
  --os=lin --prefix=kmp_i18n --default=third_party/openmp/kmp_i18n_default.inc \
  ~/vendor/llvm-project/openmp/runtime/src/i18n/en_US.txt
