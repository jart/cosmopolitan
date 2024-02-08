#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TOOL_CURL

TOOL_CURL_ARTIFACTS += TOOL_CURL_A
TOOL_CURL = $(TOOL_CURL_DEPS) $(TOOL_CURL_A)
TOOL_CURL_A = o/$(MODE)/tool/curl/curl.a
TOOL_CURL_FILES := $(wildcard tool/curl/*)
TOOL_CURL_HDRS = $(filter %.h,$(TOOL_CURL_FILES))
TOOL_CURL_INCS = $(filter %.inc,$(TOOL_CURL_FILES))
TOOL_CURL_SRCS = $(filter %.c,$(TOOL_CURL_FILES))
TOOL_CURL_OBJS = $(TOOL_CURL_SRCS:%.c=o/$(MODE)/%.o)

TOOL_CURL_DIRECTDEPS =				\
	LIBC_CALLS				\
	LIBC_FMT				\
	LIBC_INTRIN				\
	LIBC_LOG				\
	LIBC_MEM				\
	LIBC_NEXGEN32E				\
	LIBC_RUNTIME				\
	LIBC_SOCK				\
	LIBC_STDIO				\
	LIBC_STR				\
	LIBC_SYSV				\
	LIBC_TIME				\
	NET_HTTP				\
	NET_HTTPS				\
	THIRD_PARTY_GETOPT			\
	THIRD_PARTY_MBEDTLS			\
	THIRD_PARTY_MUSL

TOOL_CURL_DEPS :=				\
	$(call uniq,$(foreach x,$(TOOL_CURL_DIRECTDEPS),$($(x))))

TOOL_CURL_CHECKS =				\
	$(TOOL_CURL_A).pkg			\
	$(TOOL_CURL_HDRS:%=o/$(MODE)/%.ok)

$(TOOL_CURL_A):					\
		tool/curl/			\
		$(TOOL_CURL_A).pkg		\
		$(TOOL_CURL_OBJS)

$(TOOL_CURL_A).pkg:				\
		$(TOOL_CURL_OBJS)		\
		$(foreach x,$(TOOL_CURL_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/tool/curl/curl.com.dbg:		\
		$(TOOL_CURL)			\
		o/$(MODE)/tool/curl/cmd.o	\
		o/$(MODE)/tool/curl/curl.o	\
		$(CRT)				\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

TOOL_CURL_LIBS = $(TOOL_CURL_A)
TOOL_CURL_BINS = $(TOOL_CURL_COMS) $(TOOL_CURL_COMS:%=%.dbg)
TOOL_CURL_COMS = o/$(MODE)/tool/curl/curl.com
$(TOOL_CURL_OBJS): $(BUILD_FILES) tool/curl/BUILD.mk

.PHONY: o/$(MODE)/tool/curl
o/$(MODE)/tool/curl:				\
		$(TOOL_CURL_BINS)		\
		$(TOOL_CURL_CHECKS)
