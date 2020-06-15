#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

.PHONY:		o/$(MODE)/test/tool
o/$(MODE)/test/tool:				\
		o/$(MODE)/test/tool/build	\
		o/$(MODE)/test/tool/viz
