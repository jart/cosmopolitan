#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_LIBCXXABI

THIRD_PARTY_LIBCXXABI_ARTIFACTS += THIRD_PARTY_LIBCXXABI_A
THIRD_PARTY_LIBCXXABI = $(THIRD_PARTY_LIBCXXABI_A_DEPS) $(THIRD_PARTY_LIBCXXABI_A)
THIRD_PARTY_LIBCXXABI_A = o/$(MODE)/third_party/libcxxabi/libcxxabi.a

THIRD_PARTY_LIBCXXABI_A_INCS =						\
	third_party/libcxxabi/demangle/ItaniumNodes.def			\
	third_party/libcxxabi/aix_state_tab_eh.inc			\
	third_party/libcxxabi/cxa_exception.h				\
	third_party/libcxxabi/cxa_guard_impl.h				\
	third_party/libcxxabi/cxa_handlers.h

# TODO: Remove this aligned_alloc header when libcxx is updated
THIRD_PARTY_LIBCXXABI_A_HDRS =						\
	third_party/libcxxabi/libcxx/include/__memory/aligned_alloc.h	\
	third_party/libcxxabi/include/__cxxabi_config.h			\
	third_party/libcxxabi/include/cxxabi.h				\
	third_party/libcxxabi/demangle/DemangleConfig.h			\
	third_party/libcxxabi/demangle/ItaniumDemangle.h		\
	third_party/libcxxabi/demangle/StringViewExtras.h		\
	third_party/libcxxabi/demangle/Utility.h			\
	third_party/libcxxabi/abort_message.h				\
	third_party/libcxxabi/fallback_malloc.h				\
	third_party/libcxxabi/private_typeinfo.h

THIRD_PARTY_LIBCXXABI_A_SRCS_CC =					\
	third_party/libcxxabi/abort_message.cc				\
	third_party/libcxxabi/cxa_aux_runtime.cc			\
	third_party/libcxxabi/cxa_default_handlers.cc			\
	third_party/libcxxabi/cxa_demangle.cc				\
	third_party/libcxxabi/cxa_exception_storage.cc			\
	third_party/libcxxabi/cxa_exception.cc				\
	third_party/libcxxabi/cxa_guard.cc				\
	third_party/libcxxabi/cxa_handlers.cc				\
	third_party/libcxxabi/cxa_personality.cc			\
	third_party/libcxxabi/cxa_thread_atexit.cc			\
	third_party/libcxxabi/cxa_vector.cc				\
	third_party/libcxxabi/cxa_virtual.cc				\
	third_party/libcxxabi/fallback_malloc.cc			\
	third_party/libcxxabi/private_typeinfo.cc			\
	third_party/libcxxabi/stdlib_exception.cc			\
	third_party/libcxxabi/stdlib_new_delete.cc			\
	third_party/libcxxabi/stdlib_stdexcept.cc			\
	third_party/libcxxabi/stdlib_typeinfo.cc

THIRD_PARTY_LIBCXXABI_A_SRCS =						\
	$(THIRD_PARTY_LIBCXXABI_A_SRCS_CC)

THIRD_PARTY_LIBCXXABI_A_OBJS =						\
	$(THIRD_PARTY_LIBCXXABI_A_SRCS_CC:%.cc=o/$(MODE)/%.o)

THIRD_PARTY_LIBCXXABI_A_CHECKS =					\
	$(THIRD_PARTY_LIBCXXABI_A).pkg					\
	$(THIRD_PARTY_LIBCXXABI_A_HDRS:%=o/$(MODE)/%.okk)

THIRD_PARTY_LIBCXXABI_A_DIRECTDEPS =					\
	LIBC_CALLS							\
	LIBC_INTRIN							\
	LIBC_MEM							\
	LIBC_NEXGEN32E							\
	LIBC_RUNTIME							\
	LIBC_STDIO							\
	LIBC_STR							\
	LIBC_THREAD							\
	THIRD_PARTY_LIBUNWIND

THIRD_PARTY_LIBCXXABI_A_DEPS :=						\
	$(call uniq,$(foreach x,$(THIRD_PARTY_LIBCXXABI_A_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_LIBCXXABI_A):						\
		third_party/libcxxabi/					\
		$(THIRD_PARTY_LIBCXXABI_A).pkg				\
		$(THIRD_PARTY_LIBCXXABI_A_OBJS)

$(THIRD_PARTY_LIBCXXABI_A).pkg:						\
		$(THIRD_PARTY_LIBCXXABI_A_OBJS)				\
		$(foreach x,$(THIRD_PARTY_LIBCXXABI_A_DIRECTDEPS),$($(x)_A).pkg)

# TODO: Remove constinit hacks when we have C++20
$(THIRD_PARTY_LIBCXXABI_A_OBJS): private				\
		CXXFLAGS +=						\
			-fno-sanitize=all				\
			-ffunction-sections				\
			-fdata-sections					\
			-fexceptions					\
			-frtti						\
			-D_LIBCXXABI_BUILDING_LIBRARY			\
			-D_LIBCPP_BUILDING_LIBRARY			\
			-DHAVE___CXA_THREAD_ATEXIT_IMPL			\
			-D_LIBCPP_CONSTINIT=__constinit			\
			-Dconstinit=__constinit

# ttypeIndex is initialized and used only when native_old_exception is true.
# GCC does not seem to allow this.
o/$(MODE)/third_party/libcxxabi/cxa_personality.o: private		\
		CXXFLAGS +=						\
			-Wno-error=maybe-uninitialized

THIRD_PARTY_LIBCXXABI_LIBS = $(foreach x,$(THIRD_PARTY_LIBCXXABI_ARTIFACTS),$($(x)))
THIRD_PARTY_LIBCXXABI_SRCS = $(foreach x,$(THIRD_PARTY_LIBCXXABI_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_LIBCXXABI_HDRS = $(foreach x,$(THIRD_PARTY_LIBCXXABI_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_LIBCXXABI_INCS = $(foreach x,$(THIRD_PARTY_LIBCXXABI_ARTIFACTS),$($(x)_INCS))
THIRD_PARTY_LIBCXXABI_CHECKS = $(foreach x,$(THIRD_PARTY_LIBCXXABI_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_LIBCXXABI_OBJS = $(foreach x,$(THIRD_PARTY_LIBCXXABI_ARTIFACTS),$($(x)_OBJS))

.PHONY: o/$(MODE)/third_party/libcxxabi
o/$(MODE)/third_party/libcxxabi: 					\
	o/$(MODE)/third_party/libcxxabi/test				\
	$(THIRD_PARTY_LIBCXXABI_CHECKS)					\
	$(THIRD_PARTY_LIBCXXABI_A)
