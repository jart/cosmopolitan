#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

.PHONY:		o/$(MODE)/test/libc
o/$(MODE)/test/libc:					\
		o/$(MODE)/test/libc/calls		\
		o/$(MODE)/test/libc/fmt			\
		o/$(MODE)/test/libc/intrin		\
		o/$(MODE)/test/libc/log			\
		o/$(MODE)/test/libc/mem			\
		o/$(MODE)/test/libc/nexgen32e		\
		o/$(MODE)/test/libc/proc		\
		o/$(MODE)/test/libc/release		\
		o/$(MODE)/test/libc/runtime		\
		o/$(MODE)/test/libc/sock		\
		o/$(MODE)/test/libc/stdio		\
		o/$(MODE)/test/libc/str			\
		o/$(MODE)/test/libc/thread		\
		o/$(MODE)/test/libc/time		\
		o/$(MODE)/test/libc/tinymath		\
		o/$(MODE)/test/libc/x			\
		o/$(MODE)/test/libc/xed
