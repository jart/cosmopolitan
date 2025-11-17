#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TOOL_CHAT

TOOL_CHAT_FILES := $(wildcard tool/chat/*)
TOOL_CHAT_SRCS = $(filter %.c,$(TOOL_CHAT_FILES))
TOOL_CHAT_HDRS = $(filter %.h,$(TOOL_CHAT_FILES))
TOOL_CHAT_BINS = $(TOOL_CHAT_COMS) $(TOOL_CHAT_COMS:%=%.dbg)
TOOL_CHAT_OBJS = $(TOOL_CHAT_SRCS:%.c=o/$(MODE)/%.o)
TOOL_CHAT_COMS = $(TOOL_CHAT_SRCS:%.c=o/$(MODE)/%)
TOOL_CHAT_CHECKS = $(TOOL_CHAT).pkg $(TOOL_CHAT_HDRS:%=o/$(MODE)/%.ok)

TOOL_CHAT_DIRECTDEPS =							\
	LIBC_CALLS							\
	LIBC_INTRIN							\
	LIBC_NEXGEN32E							\
	LIBC_RUNTIME							\
	LIBC_SOCK							\
	LIBC_STDIO							\
	LIBC_STR							\
	LIBC_SYSV							\
	NET_HTTP							\
	THIRD_PARTY_HACLSTAR						\
	THIRD_PARTY_MUSL						\

TOOL_CHAT_DEPS :=							\
	$(call uniq,$(foreach x,$(TOOL_CHAT_DIRECTDEPS),$($(x))))

o/$(MODE)/tool/chat/chat.pkg:						\
		$(TOOL_CHAT_OBJS)					\
		$(foreach x,$(TOOL_CHAT_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/tool/chat/%.dbg:						\
		$(TOOL_CHAT_DEPS)					\
		o/$(MODE)/tool/chat/chat.pkg				\
		o/$(MODE)/tool/chat/%.o					\
		$(CRT)							\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

.PHONY: o/$(MODE)/tool/chat
o/$(MODE)/tool/chat:							\
		$(TOOL_CHAT_BINS)					\
		$(TOOL_CHAT_CHECKS)
