#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

.PHONY:			o/$(MODE)/test/net
o/$(MODE)/test/net:	o/$(MODE)/test/net/finger	\
			o/$(MODE)/test/net/http		\
			o/$(MODE)/test/net/https
