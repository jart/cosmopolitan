#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

o/$(MODE)/libc/aarch64/%.o: libc/aarch64/%.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<

o/$(MODE)/libc/aarch64/start.o:				\
		libc/aarch64/start.c			\
		libc/runtime/runtime.h

o/$(MODE)/libc/aarch64:					\
		o/$(MODE)/libc/aarch64/crt.o		\
		o/$(MODE)/libc/aarch64/start.o
