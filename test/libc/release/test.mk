#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

o/$(MODE)/test/libc/release/cosmopolitan.zip:			\
		o/cosmopolitan.h				\
		o/$(MODE)/ape/ape.lds				\
		o/$(MODE)/libc/crt/crt.o			\
		o/$(MODE)/ape/ape.o				\
		o/$(MODE)/cosmopolitan.a
	zip -j $@ $^

o/$(MODE)/test/libc/release/smoke.com:				\
		test/libc/release/smoke.c			\
		o/cosmopolitan.h				\
		o/$(MODE)/ape/ape.lds				\
		o/$(MODE)/libc/crt/crt.o			\
		o/$(MODE)/ape/ape.o				\
		o/$(MODE)/cosmopolitan.a
	@ACTION=CC build/compile $(CC)				\
		-o $@						\
		-Os						\
		-static						\
		-no-pie						\
		-fno-pie					\
		-nostdlib					\
		-nostdinc					\
		-mno-red-zone					\
		-Wl,--gc-sections				\
		-Wl,--oformat=binary				\
		-Wl,-z,max-page-size=0x1000			\
		-Wl,-T,o/$(MODE)/ape/ape.lds			\
		-include o/cosmopolitan.h			\
		test/libc/release/smoke.c			\
		o/$(MODE)/libc/crt/crt.o			\
		o/$(MODE)/ape/ape.o				\
		o/$(MODE)/cosmopolitan.a

.PHONY: o/$(MODE)/test/libc/release
o/$(MODE)/test/libc/release:					\
		o/$(MODE)/test/libc/release/smoke.com		\
		o/$(MODE)/test/libc/release/smoke.com.runs
