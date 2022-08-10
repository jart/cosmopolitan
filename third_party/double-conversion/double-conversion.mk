#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += THIRD_PARTY_DOUBLECONVERSION

THIRD_PARTY_DOUBLECONVERSION_ARTIFACTS += THIRD_PARTY_DOUBLECONVERSION_A

THIRD_PARTY_DOUBLECONVERSION =							\
	$(THIRD_PARTY_DOUBLECONVERSION_A_DEPS)					\
	$(THIRD_PARTY_DOUBLECONVERSION_A)

THIRD_PARTY_DOUBLECONVERSION_A =						\
	o/$(MODE)/third_party/double-conversion/libdouble-conversion.a

THIRD_PARTY_DOUBLECONVERSION_A_FILES := $(wildcard third_party/double-conversion/*)
THIRD_PARTY_DOUBLECONVERSION_A_SRCS_C = $(filter %.c,$(THIRD_PARTY_DOUBLECONVERSION_A_FILES))
THIRD_PARTY_DOUBLECONVERSION_A_SRCS_CC = $(filter %.cc,$(THIRD_PARTY_DOUBLECONVERSION_A_FILES))
THIRD_PARTY_DOUBLECONVERSION_A_HDRS = $(filter %.h,$(THIRD_PARTY_DOUBLECONVERSION_A_FILES))

THIRD_PARTY_DOUBLECONVERSION_A_SRCS =						\
	$(THIRD_PARTY_DOUBLECONVERSION_A_SRCS_C)				\
	$(THIRD_PARTY_DOUBLECONVERSION_A_SRCS_CC)

THIRD_PARTY_DOUBLECONVERSION_TEST_COMS =					\
	o/$(MODE)/third_party/double-conversion/double-conversion-tester.com

THIRD_PARTY_DOUBLECONVERSION_TEST_BINS =					\
	$(THIRD_PARTY_DOUBLECONVERSION_TEST_COMS)				\
	$(THIRD_PARTY_DOUBLECONVERSION_TEST_COMS:%=%.dbg)

THIRD_PARTY_DOUBLECONVERSION_TEST_A_SRCS_CC =					\
	third_party/double-conversion/test/cctest.cc				\
	third_party/double-conversion/test/gay-fixed.cc				\
	third_party/double-conversion/test/gay-precision.cc			\
	third_party/double-conversion/test/gay-shortest.cc			\
	third_party/double-conversion/test/gay-shortest-single.cc		\
	third_party/double-conversion/test/test-bignum.cc			\
	third_party/double-conversion/test/test-bignum-dtoa.cc			\
	third_party/double-conversion/test/test-conversions.cc			\
	third_party/double-conversion/test/test-diy-fp.cc			\
	third_party/double-conversion/test/test-dtoa.cc				\
	third_party/double-conversion/test/test-fast-dtoa.cc			\
	third_party/double-conversion/test/test-fixed-dtoa.cc			\
	third_party/double-conversion/test/test-ieee.cc				\
	third_party/double-conversion/test/test-strtod.cc

THIRD_PARTY_DOUBLECONVERSION_TEST_A_HDRS =					\
	third_party/double-conversion/test/cctest.h				\
	third_party/double-conversion/test/checks.h				\
	third_party/double-conversion/test/gay-fixed.h				\
	third_party/double-conversion/test/gay-precision.h			\
	third_party/double-conversion/test/gay-shortest.h			\
	third_party/double-conversion/test/gay-shortest-single.h

THIRD_PARTY_DOUBLECONVERSION_A_OBJS =						\
	$(THIRD_PARTY_DOUBLECONVERSION_A_SRCS_C:%.c=o/$(MODE)/%.o)		\
	$(THIRD_PARTY_DOUBLECONVERSION_A_SRCS_CC:%.cc=o/$(MODE)/%.o)

THIRD_PARTY_DOUBLECONVERSION_A_CHECKS =						\
	$(THIRD_PARTY_DOUBLECONVERSION_A).pkg					\
	$(THIRD_PARTY_DOUBLECONVERSION_A_HDRS:%=o/$(MODE)/%.okk)

THIRD_PARTY_DOUBLECONVERSION_TEST_A_OBJS =					\
	$(THIRD_PARTY_DOUBLECONVERSION_TEST_A_SRCS_CC:%.cc=o/$(MODE)/%.o)

THIRD_PARTY_DOUBLECONVERSION_A_DIRECTDEPS =					\
	LIBC_INTRIN								\
	LIBC_NEXGEN32E								\
	LIBC_MEM								\
	LIBC_RUNTIME								\
	LIBC_RAND								\
	LIBC_STDIO								\
	LIBC_FMT								\
	LIBC_SYSV								\
	LIBC_STR								\
	LIBC_STUBS								\
	LIBC_UNICODE								\
	LIBC_TINYMATH								\
	THIRD_PARTY_GDTOA							\
	THIRD_PARTY_LIBCXX

THIRD_PARTY_DOUBLECONVERSION_A_DEPS :=						\
	$(call uniq,$(foreach x,$(THIRD_PARTY_DOUBLECONVERSION_A_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_DOUBLECONVERSION_A):						\
		third_party/double-conversion/					\
		$(THIRD_PARTY_DOUBLECONVERSION_A).pkg				\
		$(THIRD_PARTY_DOUBLECONVERSION_A_OBJS)

$(THIRD_PARTY_DOUBLECONVERSION_A).pkg:						\
		$(THIRD_PARTY_DOUBLECONVERSION_A_OBJS)				\
		$(foreach x,$(THIRD_PARTY_DOUBLECONVERSION_A_DIRECTDEPS),$($(x)_A).pkg)

$(THIRD_PARTY_DOUBLECONVERSION_A_OBJS):						\
		OVERRIDE_CXXFLAGS +=						\
			-ffunction-sections					\
			-fdata-sections

o/$(MODE)/third_party/double-conversion/double-conversion-tester.com.dbg:	\
		$(THIRD_PARTY_DOUBLECONVERSION_A_DEPS)				\
		$(THIRD_PARTY_DOUBLECONVERSION_A)				\
		$(THIRD_PARTY_DOUBLECONVERSION_TEST_A_OBJS)			\
		$(CRT)								\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

THIRD_PARTY_DOUBLECONVERSION_TEST_NAMES =					\
	test-strtod								\
	test-ieee								\
	test-fixed-dtoa								\
	test-fast-dtoa								\
	test-dtoa								\
	test-diy-fp								\
	test-conversions							\
	test-bignum-dtoa							\
	test-bignum

THIRD_PARTY_DOUBLECONVERSION_TEST_RUNS =					\
	$(THIRD_PARTY_DOUBLECONVERSION_TEST_NAMES:%=o/$(MODE)/third_party/double-conversion/%.runs)

# TODO(jart): find some way to run these under runitd

o/$(MODE)/third_party/double-conversion/test-strtod.runs:			\
		o/$(MODE)/third_party/double-conversion/double-conversion-tester.com
	@$(COMPILE) -wACHECK -tT$@ $< test-strtod

o/$(MODE)/third_party/double-conversion/test-ieee.runs:				\
		o/$(MODE)/third_party/double-conversion/double-conversion-tester.com
	@$(COMPILE) -wACHECK -tT$@ $< test-ieee

o/$(MODE)/third_party/double-conversion/test-fixed-dtoa.runs:			\
		o/$(MODE)/third_party/double-conversion/double-conversion-tester.com
	@$(COMPILE) -wACHECK -tT$@ $< test-fixed-dtoa

o/$(MODE)/third_party/double-conversion/test-fast-dtoa.runs:			\
		o/$(MODE)/third_party/double-conversion/double-conversion-tester.com
	@$(COMPILE) -wACHECK -tT$@ $< test-fast-dtoa

o/$(MODE)/third_party/double-conversion/test-dtoa.runs:				\
		o/$(MODE)/third_party/double-conversion/double-conversion-tester.com
	@$(COMPILE) -wACHECK -tT$@ $< test-dtoa

o/$(MODE)/third_party/double-conversion/test-diy-fp.runs:			\
		o/$(MODE)/third_party/double-conversion/double-conversion-tester.com
	@$(COMPILE) -wACHECK -tT$@ $< test-diy-fp

o/$(MODE)/third_party/double-conversion/test-conversions.runs:			\
		o/$(MODE)/third_party/double-conversion/double-conversion-tester.com
	@$(COMPILE) -wACHECK -tT$@ $< test-conversions

o/$(MODE)/third_party/double-conversion/test-bignum-dtoa.runs:			\
		o/$(MODE)/third_party/double-conversion/double-conversion-tester.com
	@$(COMPILE) -wACHECK -tT$@ $< test-bignum-dtoa

o/$(MODE)/third_party/double-conversion/test-bignum.runs:			\
		o/$(MODE)/third_party/double-conversion/double-conversion-tester.com
	@$(COMPILE) -wACHECK -tT$@ $< test-bignum

THIRD_PARTY_DOUBLECONVERSION_LIBS = $(foreach x,$(THIRD_PARTY_DOUBLECONVERSION_ARTIFACTS),$($(x)))
THIRD_PARTY_DOUBLECONVERSION_SRCS = $(foreach x,$(THIRD_PARTY_DOUBLECONVERSION_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_DOUBLECONVERSION_HDRS = $(foreach x,$(THIRD_PARTY_DOUBLECONVERSION_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_DOUBLECONVERSION_CHECKS = $(foreach x,$(THIRD_PARTY_DOUBLECONVERSION_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_DOUBLECONVERSION_OBJS = $(foreach x,$(THIRD_PARTY_DOUBLECONVERSION_ARTIFACTS),$($(x)_OBJS))

.PHONY: o/$(MODE)/third_party/double-conversion
o/$(MODE)/third_party/double-conversion:					\
		$(THIRD_PARTY_DOUBLECONVERSION_CHECKS)				\
		$(THIRD_PARTY_DOUBLECONVERSION_TEST_RUNS)
