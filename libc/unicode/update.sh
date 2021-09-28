#!/bin/sh
[ -d libc/unicode ] || exit
[ -x o//examples/curl.com ] || make -j8 o//examples/curl.com || exit
mkdir -p o/tmp/ || exit

shineget() {
  echo $2
  o//examples/curl.com $2 >o/tmp/$$ || exit
  mv o/tmp/$$ $1 || exit
}

shineget libc/unicode/blocks.txt         https://www.unicode.org/Public/UCD/latest/ucd/Blocks.txt
shineget libc/unicode/unicodedata.txt    https://www.unicode.org/Public/UCD/latest/ucd/UnicodeData.txt
shineget libc/unicode/eastasianwidth.txt https://www.unicode.org/Public/UCD/latest/ucd/EastAsianWidth.txt
shineget libc/unicode/SpecialCasing.txt  https://www.unicode.org/Public/UCD/latest/ucd/SpecialCasing.txt
