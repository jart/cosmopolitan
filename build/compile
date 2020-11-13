#!/bin/sh
#-*-mode:sh;indent-tabs-mode:nil;tab-width:2;coding:utf-8-*-┐
#───vi: set net ft=sh ts=2 sts=2 fenc=utf-8 :vi─────────────┘
#
# OVERVIEW
#
#   GNU/LLVM Compiler Frontend Frontend
#
# DESCRIPTION
#
#   This wrapper script filters out certain flags so the compiler won't
#   whine, and passes extra information to the preprocessor.
#
# EXAMPLE
#
#   build/compile cc -o program program.c

if [ ! -d o/third_party/gcc ]; then
  third_party/gcc/unbundle.sh
fi

if [ "$1" = "clang-10" ]; then
  if ! command -v clang-10 >/dev/null; then
    shift
    set -- o/third_party/gcc/bin/x86_64-linux-musl-gcc "$@"
  fi
fi

if [ "$1" = "clang++-10" ]; then
  if ! command -v clang++-10 >/dev/null; then
    shift
    set -- o/third_party/gcc/bin/x86_64-linux-musl-g++ "$@"
  fi
fi

export LC_ALL=C
MKDIR=${MKDIR:-$(command -v mkdir) -p} || exit

GZME=
PLAT="${1%% *}"
FDIAGNOSTIC_COLOR=
CCNAME=${CCNAME:-gcc}
CCVERSION=${CCVERSION:-4}
COUNTERMAND=

# The GNU Compiler Collection passes a lot of CFLAGS to the preprocessor
# (which we call CCFLAGS) but it should pass more; and we do just that.
NOPG=0
FIRST=1
OUTARG=0
INVISIBLE=0
for x; do
  if [ $FIRST -eq 1 ]; then
    set --
    FIRST=0
  fi
  if [ $OUTARG -eq 1 ]; then
    OUTARG=0
    OUT="$x"
  fi
  case "$x" in
    -o)
      set -- "$@" "$x"
      OUTARG=1
      ;;
    -w)
      set -- "$@" "$x" -D__W__
      ;;
    -x-no-pg)
      NOPG=1
      ;;
    -pg)
      if [ $NOPG -eq 0 ] && [ $INVISIBLE -eq 0 ]; then
        set -- "$@" "$x" -D__PG__  # @see libc/macros.h
      fi
      ;;
    -mfentry)
      if [ $NOPG -eq 0 ] && [ $INVISIBLE -eq 0 ]; then
        set -- "$@" "$x" -D__MFENTRY__
      fi
      ;;
    -fomit-frame-pointer)
      INVISIBLE=1
      set -- "$@" "$x"
      ;;
    -mno-vzeroupper)
      set -- "$@" "$x" -Wa,-msse2avx -D__MNO_VZEROUPPER__
      ;;
    -fsanitize=undefined)
      set -- "$@" "$x" -D__FSANITIZE_UNDEFINED__
      COUNTERMAND="$COUNTERMAND -fno-data-sections"  # sqlite.o
      ;;
    -mnop-mcount)
      if [ "$CCNAME" = "gcc" ] && [ "$CCVERSION" -ge 6 ]; then
        set -- "$@" "$x" -D__MNOP_MCOUNT__
      fi
      ;;
    -mrecord-mcount)
      if [ "$CCNAME" = "gcc" ] && [ "$CCVERSION" -ge 6 ]; then
        set -- "$@" "$x" -D__MRECORD_MCOUNT__
      fi
      ;;
    -fsanitize=implicit*integer*)
      if ! [ "$CCNAME" = "gcc" ]; then
        set -- "$@" "$x"
      fi
      ;;
    -f*sanitize*|-gz*|-*stack-protector*|-fvect-cost*|-mstringop*)
      if [ "$CCNAME" = "gcc" ] && [ "$CCVERSION" -ge 6 ]; then
        set -- "$@" "$x"
      fi
      ;;
    -freorder-blocks-and-partition|-fstack-clash-protection)
      if [ "$CCNAME" = "gcc" ] && [ "$CCVERSION" -ge 8 ]; then
        set -- "$@" "$x"
      fi
      ;;
    -fdiagnostic-color=*)
      FDIAGNOSTIC_COLOR=$x
      ;;
    -fopt-info*=*.optinfo)
      if [ "$CCNAME" = "gcc" ] && [ "$CCVERSION" -ge 9 ]; then
        GZME="$GZME ${x##*=}"
        set -- "$@" "$x"
      fi
      ;;
    -R*)                         # e.g. clang's -Rpass-missed=all
      if [ "${PLAT#*clang}" != "${PLAT}" ]; then
        set -- "$@" "$x"
      fi
      ;;
    -fsave-optimization-record)  # clang only
      if [ "${PLAT#*clang}" != "${PLAT}" ]; then
        set -- "$@" "$x"
      fi
      ;;
    *)
      set -- "$@" "$x"
      ;;
  esac
done
if [ -z "$FDIAGNOSTIC_COLOR" ] && [ "$TERM" != "dumb" ]; then
  FDIAGNOSTIC_COLOR=-fdiagnostics-color=always
fi

if [ "${PLAT#*clang}" != "${PLAT}" ]; then
  FIRST=1
  for x; do
    # clang's assembler isn't complete yet
    if [ $FIRST -eq 1 ]; then
      set --                                \
          "$x"                              \
          -fno-integrated-as                \
          -Wno-unused-command-line-argument \
          -Wno-incompatible-pointer-types-discards-qualifiers
      FIRST=0
      continue
    fi
    TRAPV= # clang handles -f{,no-}{trap,wrap}v weird
    # removes flags clang whines about
    case "$x" in
      -gstabs) ;;
      -ftrapv) ;;
      -ffixed-*) ;;
      -fcall-saved*) ;;
      -fsignaling-nans) ;;
      -fcx-limited-range) ;;
      -fno-fp-int-builtin-inexact) ;;
      -Wno-unused-but-set-variable) ;;
      -Wunsafe-loop-optimizations) ;;
      -mdispatch-scheduler) ;;
      -ftracer) ;;
      -frounding-math) ;;
      -fmerge-constants) ;;
      -fmodulo-sched) ;;
      -msse2avx)
        set -- "$@" -Wa,-msse2avx
        ;;
      -fopt-info-vec) ;;
      -fopt-info-vec-missed) ;;
      -fmodulo-sched-allow-regmoves) ;;
      -fgcse-*) ;;
      -freschedule-modulo-scheduled-loops) ;;
      -freschedule-modulo-scheduled-loops) ;;
      -fipa-pta) ;;
      -fsched2-use-superblocks) ;;
      -fbranch-target-load-optimize) ;;
      -fdelete-dead-exceptions) ;;
      -funsafe-loop-optimizations) ;;
      -fcall-used*) ;;
      -mmitigate-rop) ;;
      -mnop-mcount) ;;
      -fno-align-jumps) ;;
      -fno-align-labels) ;;
      -fno-align-loops) ;;
      -fivopts) ;;
      -fschedule-insns) ;;
      -fno-semantic-interposition) ;;
      -mno-fentry) ;;
      -f*shrink-wrap) ;;
      -f*schedule-insns2) ;;
      -fvect-cost-model=*) ;;
      -fsimd-cost-model=*) ;;
      -fversion-loops-for-strides) ;;
      -fopt-info*) ;;
      -f*var-tracking-assignments) ;;
      -femit-struct-debug-baseonly) ;;
      -ftree-loop-vectorize) ;;
      -gdescribe-dies) ;;
      -flimit-function-alignment) ;;
      -ftree-loop-im) ;;
      -fno-instrument-functions) ;;
      -fstack-clash-protection) ;;
      -mstringop-strategy=*) ;;
      -mpreferred-stack-boundary=*) ;;
      -*stack-protector*) ;;  # clang requires segmented memory for this
      -f*gnu-unique) ;;
      -Wframe-larger-than=*) ;;
      -f*whole-program) ;;
      -Wa,--size-check=*) ;;
      -Wa,--listing*) ;;
      -mfpmath=sse+387) ;;
      -Wa,--noexecstack) ;;
      -freg-struct-return) ;;
      -mcall-ms2sysv-xlogues) ;;
      -mno-vzeroupper)
        set -- "$@" -mllvm -x86-use-vzeroupper=0
        ;;
      -Wa,-a*)
        x="${x#*=}"
        if [ "$x" ] && [ -p "$x" ]; then
          printf '' >"$x"
        fi
        ;;
      *)
        set -- "$@" "$x"
        ;;
    esac
  done
  set -- "$@" -fno-stack-protector
else
  # removes flags only clang supports
  FIRST=1
  for x; do
    if [ $FIRST -eq 1 ]; then
      set --
      FIRST=0
    fi
    case "$x" in
      -Oz) set -- "$@" -Os ;;
      *) set -- "$@" "$x" ;;
    esac
  done
fi

set -- "$@" -no-canonical-prefixes $FDIAGNOSTIC_COLOR $COUNTERMAND

if [ "$SILENT" = "0" ]; then
  printf "%s\n" "$*" >&2
else
  printf "$LOGFMT" "${ACTION:-COMPILE}" "${TARGET:-$OUT}" >&2
fi

OUTDIR="${OUT%/*}"
if [ "$OUTDIR" != "$OUT" ] && [ ! -d "$OUTDIR" ]; then
  $MKDIR "$OUTDIR" || exit 2
fi

REASON=failed
trap REASON=interrupted INT

if "$@"; then
  for f in $GZME; do
    if GZ=${GZ:-$(command -v gzip)}; then
      if [ -f "$f" ]; then
        build/actuallynice $GZ $ZFLAGS -qf $f &
      fi
    fi
  done
  exit 0
fi

printf "\n$LOGFMT" "$CCNAME $CCVERSION: compile $REASON:" "$*" >&2
exit 1
