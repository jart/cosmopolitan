#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += THIRD_PARTY_TBB

THIRD_PARTY_TBB_ARTIFACTS += THIRD_PARTY_TBB_A
THIRD_PARTY_TBB = $(THIRD_PARTY_TBB_A_DEPS) $(THIRD_PARTY_TBB_A)
THIRD_PARTY_TBB_A = o/$(MODE)/third_party/tbb/tbb.a
THIRD_PARTY_TBB_FILES := $(wildcard third_party/tbb/*) $(wildcard third_party/tbb/detail/*)
THIRD_PARTY_TBB_HDRS = $(filter %.h,$(THIRD_PARTY_TBB_FILES))
THIRD_PARTY_TBB_SRCS = $(filter %.cpp,$(THIRD_PARTY_TBB_FILES))
THIRD_PARTY_TBB_OBJS = $(THIRD_PARTY_TBB_SRCS:%.cpp=o/$(MODE)/%.o)

# Use this to debug
# $(info $$THIRD_PARTY_TBB_HDRS is [${THIRD_PARTY_TBB_HDRS}])

THIRD_PARTY_TBB_CHECKS =				\
	$(THIRD_PARTY_TBB_A).pkg			\
	$(THIRD_PARTY_TBB_HDRS:%=o/$(MODE)/%.ok)

THIRD_PARTY_TBB_A_DIRECTDEPS =				\
	THIRD_PARTY_LIBCXX

THIRD_PARTY_TBB_A_DEPS :=				\
	$(call uniq,$(foreach x,$(THIRD_PARTY_TBB_A_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_TBB_A):					\
		third_party/tbb/			\
		$(THIRD_PARTY_TBB_A).pkg		\
		$(THIRD_PARTY_TBB_OBJS)

$(THIRD_PARTY_TBB_A).pkg:				\
		$(THIRD_PARTY_TBB_OBJS)		\
		$(foreach x,$(THIRD_PARTY_TBB_A_DIRECTDEPS),$($(x)_A).pkg)

THIRD_PARTY_TBB_LIBS = $(THIRD_PARTY_TBB_A)

$(THIRD_PARTY_TBB_OBJS): $(BUILD_FILES) third_party/tbb/tbb.mk

.PHONY: o/$(MODE)/third_party/tbb
o/$(MODE)/third_party/tbb: \
		$(THIRD_PARTY_TBB_CHECKS) \
		$(THIRD_PARTY_TBB_A)
