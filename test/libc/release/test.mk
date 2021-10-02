#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

o/$(MODE)/test/libc/release/cosmopolitan.zip:			\
		o/cosmopolitan.h				\
		o/$(MODE)/ape/ape.lds				\
		o/$(MODE)/libc/crt/crt.o			\
		o/$(MODE)/ape/ape.o				\
		o/$(MODE)/ape/ape-no-modify-self.o		\
		o/$(MODE)/cosmopolitan.a			\
		o/$(MODE)/third_party/infozip/zip.com
	@$(COMPILE) -AZIP -T$@ o/$(MODE)/third_party/infozip/zip.com -qj $@ o/cosmopolitan.h o/$(MODE)/ape/ape.lds o/$(MODE)/libc/crt/crt.o o/$(MODE)/ape/ape.o o/$(MODE)/ape/ape-no-modify-self.o o/$(MODE)/cosmopolitan.a

o/$(MODE)/test/libc/release/smoke.com:				\
		o/$(MODE)/test/libc/release/smoke.com.dbg
	@$(COMPILE) -AOBJCOPY -T$< $(OBJCOPY) -S -O binary $< $@

o/$(MODE)/test/libc/release/smoke.com.dbg:			\
		test/libc/release/smoke.c			\
		o/cosmopolitan.h				\
		o/$(MODE)/ape/ape.lds				\
		o/$(MODE)/libc/crt/crt.o			\
		o/$(MODE)/ape/ape.o				\
		o/$(MODE)/cosmopolitan.a
	@$(COMPILE) -ACC $(CC)					\
		-o $@						\
		-Os						\
		-static						\
		-no-pie						\
		-fno-pie					\
		-nostdlib					\
		-nostdinc					\
		-mno-red-zone					\
		-fno-omit-frame-pointer				\
		-Wl,-T,o/$(MODE)/ape/ape.lds			\
		-include o/cosmopolitan.h			\
		test/libc/release/smoke.c			\
		o/$(MODE)/libc/crt/crt.o			\
		o/$(MODE)/ape/ape.o				\
		o/$(MODE)/cosmopolitan.a

o/$(MODE)/test/libc/release/smoke-nms.com.dbg:			\
		test/libc/release/smoke.c			\
		o/cosmopolitan.h				\
		o/$(MODE)/ape/ape.lds				\
		o/$(MODE)/libc/crt/crt.o			\
		o/$(MODE)/ape/ape-no-modify-self.o		\
		o/$(MODE)/cosmopolitan.a
	@$(COMPILE) -ACC $(CC)					\
		-o $@						\
		-Os						\
		-static						\
		-no-pie						\
		-fno-pie					\
		-nostdlib					\
		-nostdinc					\
		-mno-red-zone					\
		-fno-omit-frame-pointer				\
		-Wl,-T,o/$(MODE)/ape/ape.lds			\
		-include o/cosmopolitan.h			\
		test/libc/release/smoke.c			\
		o/$(MODE)/libc/crt/crt.o			\
		o/$(MODE)/ape/ape-no-modify-self.o		\
		o/$(MODE)/cosmopolitan.a

o/$(MODE)/test/libc/release/smokecxx.com:			\
		o/$(MODE)/test/libc/release/smokecxx.com.dbg
	@$(COMPILE) -AOBJCOPY -T$< $(OBJCOPY) -S -O binary $< $@

o/$(MODE)/test/libc/release/smokecxx.com.dbg:			\
		test/libc/release/smokecxx.cc			\
		o/cosmopolitan.h				\
		o/$(MODE)/ape/ape.lds				\
		o/$(MODE)/libc/crt/crt.o			\
		o/$(MODE)/ape/ape.o				\
		o/$(MODE)/cosmopolitan.a
	@$(COMPILE) -ACXX $(CXX)				\
		-o $@						\
		-Os						\
		-static						\
		-no-pie						\
		-fno-pie					\
		-nostdlib					\
		-nostdinc					\
		-mno-red-zone					\
		-fno-omit-frame-pointer				\
		-Wl,-T,o/$(MODE)/ape/ape.lds			\
		-include o/cosmopolitan.h			\
		test/libc/release/smokecxx.cc			\
		o/$(MODE)/libc/crt/crt.o			\
		o/$(MODE)/ape/ape.o				\
		o/$(MODE)/cosmopolitan.a

o/$(MODE)/test/libc/release/smokeansi.com.dbg:			\
		test/libc/release/smoke.c			\
		o/cosmopolitan.h				\
		o/$(MODE)/ape/ape.lds				\
		o/$(MODE)/libc/crt/crt.o			\
		o/$(MODE)/ape/ape.o				\
		o/$(MODE)/cosmopolitan.a
	@$(COMPILE) -AANSI $(CC)				\
		-o $@						\
		-Os						\
		-ansi						\
		-static						\
		-no-pie						\
		-fno-pie					\
		-nostdlib					\
		-nostdinc					\
		-mno-red-zone					\
		-fno-omit-frame-pointer				\
		-Wl,-T,o/$(MODE)/ape/ape.lds			\
		-include o/cosmopolitan.h			\
		test/libc/release/smoke.c			\
		o/$(MODE)/libc/crt/crt.o			\
		o/$(MODE)/ape/ape.o				\
		o/$(MODE)/cosmopolitan.a

o/$(MODE)/test/libc/release/clang.ok:				\
		test/libc/release/clang.sh			\
		test/libc/release/smoke.c			\
		o/cosmopolitan.h				\
		o/$(MODE)/ape/ape.lds				\
		o/$(MODE)/libc/crt/crt.o			\
		o/$(MODE)/ape/ape.o				\
		o/$(MODE)/cosmopolitan.a
	@$(COMPILE) -ASHTEST -tT$@ $<

o/$(MODE)/test/libc/release/lld.ok:				\
		test/libc/release/lld.sh			\
		test/libc/release/smoke.c			\
		o/cosmopolitan.h				\
		o/$(MODE)/ape/ape.lds				\
		o/$(MODE)/libc/crt/crt.o			\
		o/$(MODE)/ape/ape.o				\
		o/$(MODE)/cosmopolitan.a
	@$(COMPILE) -ASHTEST -tT$@ $<

o/$(MODE)/test/libc/release/metal.ok:				\
		test/libc/release/metal.sh			\
		o/$(MODE)/examples/hello.com			\
		o/$(MODE)/tool/build/blinkenlights.com.dbg
	@$(COMPILE) -ASHTEST -tT$@ $<

o/$(MODE)/test/libc/release/emulate.ok:				\
		test/libc/release/emulate.sh			\
		o/$(MODE)/examples/hello.com			\
		o/$(MODE)/tool/build/blinkenlights.com.dbg
	@$(COMPILE) -ASHTEST -tT$@ $<

.PHONY: o/$(MODE)/test/libc/release
o/$(MODE)/test/libc/release:					\
		o/$(MODE)/test/libc/release/smoke.com		\
		o/$(MODE)/test/libc/release/smoke.com.runs	\
		o/$(MODE)/test/libc/release/smoke-nms.com	\
		o/$(MODE)/test/libc/release/smoke-nms.com.runs	\
		o/$(MODE)/test/libc/release/smokecxx.com	\
		o/$(MODE)/test/libc/release/smokecxx.com.runs	\
		o/$(MODE)/test/libc/release/smokeansi.com	\
		o/$(MODE)/test/libc/release/smokeansi.com.runs	\
		o/$(MODE)/test/libc/release/clang.ok		\
		o/$(MODE)/test/libc/release/lld.ok		\
		o/$(MODE)/test/libc/release/emulate.ok		\
		o/$(MODE)/test/libc/release/metal.ok
