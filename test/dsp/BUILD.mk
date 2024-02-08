#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

.PHONY:			o/$(MODE)/test/dsp
o/$(MODE)/test/dsp:	o/$(MODE)/test/dsp/core		\
			o/$(MODE)/test/dsp/scale	\
			o/$(MODE)/test/dsp/tty
