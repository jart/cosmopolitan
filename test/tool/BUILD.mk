#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

.PHONY:		o/$(MODE)/test/tool
o/$(MODE)/test/tool:				\
		o/$(MODE)/test/tool/args	\
		o/$(MODE)/test/tool/build	\
		o/$(MODE)/test/tool/plinko	\
		o/$(MODE)/test/tool/net		\
		o/$(MODE)/test/tool/viz
