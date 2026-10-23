#!/bin/sh
# cosmopolitan fat ape builder
#
# Builds a fat APE binary containing both x86_64 and aarch64 native code.
#
# Usage:
#   build/fat.sh tool/net/redbean

set -ex

if [ "$#" -eq 0 ]; then
  echo "Usage: $0 TARGET" >&2
  echo "Example: $0 tool/net/redbean" >&2
  exit 1
fi

mode() {
  case $(uname -m) in
    arm64|aarch64)  echo aarch64  ;;
    *)              echo          ;;
  esac
}

TARGET="${1%.dbg}"
TARGET="${TARGET%.com}"
TARGET="${TARGET#o//}"
TARGET="${TARGET#o/x86_64/}"
TARGET="${TARGET#o/aarch64/}"
TARGET="${TARGET#o/fat/}"

AMD64="${2:-x86_64}"
ARM64="${3:-aarch64}"
APELINK="o/$(mode)/tool/build/apelink"

_nproc() {
  case $(uname -s) in
    Darwin) sysctl -n hw.logicalcpu 2>/dev/null || echo 1 ;;
    *)      nproc 2>/dev/null || echo 1                   ;;
  esac
}

if ! MAKE=$(command -v gmake); then
  if ! MAKE=$(command -v make); then
    echo "Please install gnu make" >&2
    exit 1
  fi
fi

NPROC=$(_nproc)

echo "[-] Building x86_64 target..."
$MAKE -j$NPROC m=$AMD64 "o/$AMD64/ape/ape.elf" "o/$AMD64/$TARGET.dbg"

echo "[-] Building aarch64 target..."
$MAKE -j$NPROC m=$ARM64 "o/$ARM64/ape/ape.elf" "o/$ARM64/$TARGET.dbg"

echo "[-] Building apelink tool..."
$MAKE -j$NPROC m= "$APELINK"

echo "[-] Merging Fat APE binary..."
mkdir -p "o/fat/$(dirname "$TARGET")"
"$APELINK" \
  -l "o/$AMD64/ape/ape.elf" \
  -l "o/$ARM64/ape/ape.elf" \
  -M ape/ape-m1.c \
  -o "o/fat/$TARGET.com" \
  "o/$AMD64/$TARGET.dbg" \
  "o/$ARM64/$TARGET.dbg"

echo "[+] Success: o/fat/$TARGET.com is ready for both x86_64 and aarch64."
