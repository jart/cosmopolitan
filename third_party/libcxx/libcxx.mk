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
	third_party/libcxx/__mutex_base				\
	third_party/libcxx/__node_handle			\
	third_party/libcxx/__nullptr				\
	third_party/libcxx/__split_buffer			\
	third_party/libcxx/__sso_allocator			\
	third_party/libcxx/__std_stream				\
	third_party/libcxx/__threading_support			\
	third_party/libcxx/__tuple				\
	third_party/libcxx/__undef_macros			\
	third_party/libcxx/algorithm				\
	third_party/libcxx/array				\
	third_party/libcxx/atomic_support.hh			\
	third_party/libcxx/bit					\
	third_party/libcxx/bitset				\
	third_party/libcxx/cassert				\
	third_party/libcxx/cerrno				\
	third_party/libcxx/chrono				\
	third_party/libcxx/climits				\
	third_party/libcxx/cmath				\
	third_party/libcxx/codecvt				\
	third_party/libcxx/condition_variable			\
	third_party/libcxx/config_elast.h			\
	third_party/libcxx/cstddef				\
	third_party/libcxx/cstdint				\
	third_party/libcxx/cstdio				\
	third_party/libcxx/cstdlib				\
	third_party/libcxx/cstring				\
	third_party/libcxx/deque				\
	third_party/libcxx/exception				\
	third_party/libcxx/exception_fallback.hh		\
	third_party/libcxx/exception_pointer_unimplemented.hh	\
	third_party/libcxx/include/atomic_support.hh		\
	third_party/libcxx/include/config_elast.hh		\
	third_party/libcxx/initializer_list			\
	third_party/libcxx/ios					\
	third_party/libcxx/iosfwd				\
	third_party/libcxx/iostream				\
	third_party/libcxx/istream				\
	third_party/libcxx/limits				\
	third_party/libcxx/list					\
	third_party/libcxx/map					\
	third_party/libcxx/memory				\
	third_party/libcxx/mutex				\
	third_party/libcxx/new					\
	third_party/libcxx/new_handler_fallback.hh		\
	third_party/libcxx/numeric				\
	third_party/libcxx/optional				\
	third_party/libcxx/ostream				\
	third_party/libcxx/queue				\
	third_party/libcxx/queue				\
	third_party/libcxx/random				\
	third_party/libcxx/ratio				\
	third_party/libcxx/refstring.hh				\
	third_party/libcxx/set					\
	third_party/libcxx/sstream				\
	third_party/libcxx/stdexcept				\
	third_party/libcxx/stdexcept_default.hh			\
	third_party/libcxx/streambuf				\
	third_party/libcxx/string				\
	third_party/libcxx/string_view				\
	third_party/libcxx/system_error				\
	third_party/libcxx/tuple				\
	third_party/libcxx/type_traits				\
	third_party/libcxx/typeinfo				\
	third_party/libcxx/unordered_map			\
	third_party/libcxx/unordered_set			\
	third_party/libcxx/utility				\
	third_party/libcxx/variant				\
	third_party/libcxx/vector				\
	third_party/libcxx/version				\
	third_party/libcxx/wchar.h

THIRD_PARTY_LIBCXX_A_SRCS_CC =					\
	third_party/libcxx/algorithm.cc				\
	third_party/libcxx/charconv.cc				\
	third_party/libcxx/chrono.cc				\
	third_party/libcxx/condition_variable.cc		\
	third_party/libcxx/condition_variable_destructor.cc	\
	third_party/libcxx/exception.cc				\
	third_party/libcxx/functional.cc			\
	third_party/libcxx/hash.cc				\
	third_party/libcxx/ios.cc				\
	third_party/libcxx/iostream.cc				\
	third_party/libcxx/locale.cc				\
	third_party/libcxx/memory.cc				\
	third_party/libcxx/mutex.cc				\
	third_party/libcxx/new.cc				\
	third_party/libcxx/optional.cc				\
	third_party/libcxx/random.cc				\
	third_party/libcxx/stdexcept.cc				\
	third_party/libcxx/string.cc				\
	third_party/libcxx/system_error.cc			\
	third_party/libcxx/vector.cc

THIRD_PARTY_LIBCXX_A_SRCS =					\
	$(THIRD_PARTY_LIBCXX_A_SRCS_S)				\
	$(THIRD_PARTY_LIBCXX_A_SRCS_CC)

THIRD_PARTY_LIBCXX_A_OBJS =					\
	$(THIRD_PARTY_LIBCXX_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(THIRD_PARTY_LIBCXX_A_SRCS_C:%.c=o/$(MODE)/%.o)	\
	$(THIRD_PARTY_LIBCXX_A_SRCS_CC:%.cc=o/$(MODE)/%.o)

THIRD_PARTY_LIBCXX_A_CHECKS =					\
	$(THIRD_PARTY_LIBCXX_A).pkg				\
	$(THIRD_PARTY_LIBCXX_A_HDRS:%=o/$(MODE)/%.okk)

THIRD_PARTY_LIBCXX_A_DIRECTDEPS =				\
	LIBC_CALLS						\
	LIBC_FMT						\
	LIBC_INTRIN						\
	LIBC_MEM						\
	LIBC_NEXGEN32E						\
	LIBC_RAND						\
	LIBC_RUNTIME						\
	LIBC_STDIO						\
	LIBC_STR						\
	LIBC_STUBS						\
	LIBC_SYSV						\
	LIBC_TIME						\
	LIBC_TINYMATH						\
	LIBC_UNICODE						\
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

o/$(MODE)/third_party/libcxx/locale.o: QUOTA = -C32 -M1024m

THIRD_PARTY_LIBCXX_LIBS = $(foreach x,$(THIRD_PARTY_LIBCXX_ARTIFACTS),$($(x)))
THIRD_PARTY_LIBCXX_SRCS = $(foreach x,$(THIRD_PARTY_LIBCXX_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_LIBCXX_HDRS = $(foreach x,$(THIRD_PARTY_LIBCXX_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_LIBCXX_INCS = $(foreach x,$(THIRD_PARTY_LIBCXX_ARTIFACTS),$($(x)_INCS))
THIRD_PARTY_LIBCXX_CHECKS = $(foreach x,$(THIRD_PARTY_LIBCXX_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_LIBCXX_OBJS = $(foreach x,$(THIRD_PARTY_LIBCXX_ARTIFACTS),$($(x)_OBJS))

.PHONY: o/$(MODE)/third_party/libcxx
o/$(MODE)/third_party/libcxx: $(THIRD_PARTY_LIBCXX_CHECKS)

