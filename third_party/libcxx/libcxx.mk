#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += THIRD_PARTY_LIBCXX

THIRD_PARTY_LIBCXX_ARTIFACTS += THIRD_PARTY_LIBCXX_A
THIRD_PARTY_LIBCXX = $(THIRD_PARTY_LIBCXX_A_DEPS) $(THIRD_PARTY_LIBCXX_A)
THIRD_PARTY_LIBCXX_A = o/$(MODE)/third_party/libcxx/libcxx.a

THIRD_PARTY_LIBCXX_A_HDRS =					\
	third_party/libcxx/__bit_reference			\
	third_party/libcxx/__config				\
	third_party/libcxx/__debug				\
	third_party/libcxx/__functional_base			\
	third_party/libcxx/__hash_table				\
	third_party/libcxx/__node_handle			\
	third_party/libcxx/__nullptr				\
	third_party/libcxx/__split_buffer			\
	third_party/libcxx/__tuple				\
	third_party/libcxx/__undef_macros			\
	third_party/libcxx/algorithm				\
	third_party/libcxx/bitset				\
	third_party/libcxx/cassert				\
	third_party/libcxx/cerrno				\
	third_party/libcxx/climits				\
	third_party/libcxx/cmath				\
	third_party/libcxx/cstddef				\
	third_party/libcxx/cstdint				\
	third_party/libcxx/cstdio				\
	third_party/libcxx/cstdlib				\
	third_party/libcxx/cstring				\
	third_party/libcxx/deque				\
	third_party/libcxx/initializer_list			\
	third_party/libcxx/iosfwd				\
	third_party/libcxx/limits				\
	third_party/libcxx/list					\
	third_party/libcxx/map					\
	third_party/libcxx/memory				\
	third_party/libcxx/optional				\
	third_party/libcxx/queue				\
	third_party/libcxx/set					\
	third_party/libcxx/stdexcept				\
	third_party/libcxx/string				\
	third_party/libcxx/type_traits				\
	third_party/libcxx/unordered_map			\
	third_party/libcxx/unordered_set			\
	third_party/libcxx/vector				\
	third_party/libcxx/version				\
	third_party/libcxx/wchar.h				\
	third_party/libcxx/config_elast.h			\
	third_party/libcxx/atomic_support.hh			\
	third_party/libcxx/exception_fallback.hh		\
	third_party/libcxx/exception_pointer_unimplemented.hh	\
	third_party/libcxx/new_handler_fallback.hh		\
	third_party/libcxx/refstring.hh				\
	third_party/libcxx/stdexcept_default.hh

THIRD_PARTY_LIBCXX_A_SRCS_CC =					\
	third_party/libcxx/algorithm.cc				\
	third_party/libcxx/charconv.cc				\
	third_party/libcxx/functional.cc			\
	third_party/libcxx/system_error.cc			\
	third_party/libcxx/random.cc				\
	third_party/libcxx/hash.cc				\
	third_party/libcxx/string.cc				\
	third_party/libcxx/vector.cc				\
	third_party/libcxx/stdexcept.cc				\
	third_party/libcxx/exception.cc

THIRD_PARTY_LIBCXX_A_SRCS =					\
	$(THIRD_PARTY_LIBCXX_A_SRCS_S)				\
	$(THIRD_PARTY_LIBCXX_A_SRCS_C)

THIRD_PARTY_LIBCXX_A_OBJS =					\
	$(THIRD_PARTY_LIBCXX_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(THIRD_PARTY_LIBCXX_A_SRCS_C:%.c=o/$(MODE)/%.o)	\
	$(THIRD_PARTY_LIBCXX_A_SRCS_CC:%.cc=o/$(MODE)/%.o)

THIRD_PARTY_LIBCXX_A_CHECKS =					\
	$(THIRD_PARTY_LIBCXX_A).pkg				\
	$(THIRD_PARTY_LIBCXX_A_HDRS:%=o/$(MODE)/%.okk)

THIRD_PARTY_LIBCXX_A_DIRECTDEPS =				\
	LIBC_INTRIN						\
	LIBC_NEXGEN32E						\
	LIBC_MEM						\
	LIBC_RUNTIME						\
	LIBC_RAND						\
	LIBC_STDIO						\
	LIBC_FMT						\
	LIBC_SYSV						\
	LIBC_STR						\
	LIBC_STUBS						\
	LIBC_TINYMATH						\
	THIRD_PARTY_GDTOA

THIRD_PARTY_LIBCXX_A_DEPS :=					\
	$(call uniq,$(foreach x,$(THIRD_PARTY_LIBCXX_A_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_LIBCXX_A):					\
		third_party/libcxx/				\
		$(THIRD_PARTY_LIBCXX_A).pkg			\
		$(THIRD_PARTY_LIBCXX_A_OBJS)

$(THIRD_PARTY_LIBCXX_A).pkg:					\
		$(THIRD_PARTY_LIBCXX_A_OBJS)			\
		$(foreach x,$(THIRD_PARTY_LIBCXX_A_DIRECTDEPS),$($(x)_A).pkg)

$(THIRD_PARTY_LIBCXX_A_OBJS):					\
		OVERRIDE_CXXFLAGS +=				\
			-ffunction-sections			\
			-fdata-sections

THIRD_PARTY_LIBCXX_LIBS = $(foreach x,$(THIRD_PARTY_LIBCXX_ARTIFACTS),$($(x)))
THIRD_PARTY_LIBCXX_SRCS = $(foreach x,$(THIRD_PARTY_LIBCXX_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_LIBCXX_HDRS = $(foreach x,$(THIRD_PARTY_LIBCXX_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_LIBCXX_INCS = $(foreach x,$(THIRD_PARTY_LIBCXX_ARTIFACTS),$($(x)_INCS))
THIRD_PARTY_LIBCXX_CHECKS = $(foreach x,$(THIRD_PARTY_LIBCXX_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_LIBCXX_OBJS = $(foreach x,$(THIRD_PARTY_LIBCXX_ARTIFACTS),$($(x)_OBJS))

.PHONY: o/$(MODE)/third_party/libcxx
o/$(MODE)/third_party/libcxx: $(THIRD_PARTY_LIBCXX_CHECKS)

