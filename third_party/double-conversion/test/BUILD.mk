#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_DOUBLECONVERSION_TEST

THIRD_PARTY_DOUBLECONVERSION_TEST_COMS =					\
	o/$(MODE)/third_party/double-conversion/double-conversion-tester.com

THIRD_PARTY_DOUBLECONVERSION_TEST_BINS =					\
	$(THIRD_PARTY_DOUBLECONVERSION_TEST_COMS)				\
	$(THIRD_PARTY_DOUBLECONVERSION_TEST_COMS:%=%.dbg)

THIRD_PARTY_DOUBLECONVERSION_TEST_HDRS =					\
	third_party/double-conversion/test/cctest.h				\
	third_party/double-conversion/test/checks.h				\
	third_party/double-conversion/test/gay-fixed.h				\
	third_party/double-conversion/test/gay-precision.h			\
	third_party/double-conversion/test/gay-shortest.h			\
	third_party/double-conversion/test/gay-shortest-single.h

THIRD_PARTY_DOUBLECONVERSION_TEST_SRCS =					\
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

THIRD_PARTY_DOUBLECONVERSION_TEST_OBJS =					\
	$(THIRD_PARTY_DOUBLECONVERSION_TEST_SRCS:%.cc=o/$(MODE)/%.o)

THIRD_PARTY_DOUBLECONVERSION_TEST_DIRECTDEPS =					\
	LIBC_INTRIN								\
	LIBC_NEXGEN32E								\
	LIBC_RUNTIME								\
	LIBC_FMT								\
	LIBC_SYSV								\
	LIBC_STR								\
	LIBC_TINYMATH								\
	THIRD_PARTY_LIBCXX							\
	THIRD_PARTY_DOUBLECONVERSION

THIRD_PARTY_DOUBLECONVERSION_TEST_DEPS :=					\
	$(call uniq,$(foreach x,$(THIRD_PARTY_DOUBLECONVERSION_TEST_DIRECTDEPS),$($(x))))

o/$(MODE)/third_party/double-conversion/double-conversion-tester.com.dbg:	\
		$(THIRD_PARTY_DOUBLECONVERSION_TEST_DEPS)			\
		$(THIRD_PARTY_DOUBLECONVERSION_TEST_OBJS)			\
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

THIRD_PARTY_DOUBLECONVERSION_TEST_CHECKS =					\
	$(THIRD_PARTY_DOUBLECONVERSION_TEST_NAMES:%=o/$(MODE)/third_party/double-conversion/%.runs)

# TODO(jart): find some way to run these under runitd

o/$(MODE)/third_party/double-conversion/test-strtod.runs:			\
		o/$(MODE)/third_party/double-conversion/double-conversion-tester.com
	@$(COMPILE) -ACHECK -wtT$@ $< test-strtod

o/$(MODE)/third_party/double-conversion/test-ieee.runs:				\
		o/$(MODE)/third_party/double-conversion/double-conversion-tester.com
	@$(COMPILE) -ACHECK -wtT$@ $< test-ieee

o/$(MODE)/third_party/double-conversion/test-fixed-dtoa.runs:			\
		o/$(MODE)/third_party/double-conversion/double-conversion-tester.com
	@$(COMPILE) -ACHECK -wtT$@ $< test-fixed-dtoa

o/$(MODE)/third_party/double-conversion/test-fast-dtoa.runs:			\
		o/$(MODE)/third_party/double-conversion/double-conversion-tester.com
	@$(COMPILE) -ACHECK -wtT$@ $< test-fast-dtoa

o/$(MODE)/third_party/double-conversion/test-dtoa.runs:				\
		o/$(MODE)/third_party/double-conversion/double-conversion-tester.com
	@$(COMPILE) -ACHECK -wtT$@ $< test-dtoa

o/$(MODE)/third_party/double-conversion/test-diy-fp.runs:			\
		o/$(MODE)/third_party/double-conversion/double-conversion-tester.com
	@$(COMPILE) -ACHECK -wtT$@ $< test-diy-fp

o/$(MODE)/third_party/double-conversion/test-conversions.runs:			\
		o/$(MODE)/third_party/double-conversion/double-conversion-tester.com
	@$(COMPILE) -ACHECK -wtT$@ $< test-conversions

o/$(MODE)/third_party/double-conversion/test-bignum-dtoa.runs:			\
		o/$(MODE)/third_party/double-conversion/double-conversion-tester.com
	@$(COMPILE) -ACHECK -wtT$@ $< test-bignum-dtoa

o/$(MODE)/third_party/double-conversion/test-bignum.runs:			\
		o/$(MODE)/third_party/double-conversion/double-conversion-tester.com
	@$(COMPILE) -ACHECK -wtT$@ $< test-bignum

.PHONY: o/$(MODE)/third_party/double-conversion/test
o/$(MODE)/third_party/double-conversion/test:					\
		$(THIRD_PARTY_DOUBLECONVERSION_TEST_CHECKS)
