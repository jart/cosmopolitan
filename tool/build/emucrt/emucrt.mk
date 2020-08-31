#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TOOL_BUILD_EMUCRT

TOOL_BUILD_EMUCRT =					\
	o/$(MODE)/tool/build/emucrt/emucrt.o		\
	o/$(MODE)/tool/build/emucrt/emucrt.lds

.PHONY: o/$(MODE)/tool/build/emucrt
o/$(MODE)/tool/build/emucrt:				\
		$(TOOL_BUILD_EMUCRT)
