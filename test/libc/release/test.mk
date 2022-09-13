#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

o/$(MODE)/test/libc/release/cosmopolitan.zip: private .UNSANDBOXED = 1
o/$(MODE)/test/libc/release/cosmopolitan.zip:			\
		o/cosmopolitan.h				\
		o/$(MODE)/ape/public/ape.lds			\
		o/$(MODE)/libc/crt/crt.o			\
		o/$(MODE)/ape/ape.o				\
		o/$(MODE)/ape/ape-copy-self.o			\
		o/$(MODE)/ape/ape-no-modify-self.o		\
		o/$(MODE)/cosmopolitan.a			\
		o/$(MODE)/third_party/zip/zip.com
	@$(COMPILE) -AZIP -T$@					\
		o/$(MODE)/third_party/zip/zip.com		\
		-b$(TMPDIR) -qj $@				\
		o/cosmopolitan.h				\
		o/$(MODE)/ape/public/ape.lds			\
		o/$(MODE)/libc/crt/crt.o			\
		o/$(MODE)/ape/ape.o				\
		o/$(MODE)/ape/ape-copy-self.o			\
		o/$(MODE)/ape/ape-no-modify-self.o		\
		o/$(MODE)/cosmopolitan.a

o/$(MODE)/test/libc/release/smoke.o:					\
		test/libc/release/smoke.c				\
		o/cosmopolitan.h
	@$(COMPILE) -ACC $(CC)						\
		-o $@							\
		-c							\
		-Os							\
		-fno-pie						\
		-nostdinc						\
		-fno-omit-frame-pointer					\
		-include o/cosmopolitan.h				\
		$<

o/$(MODE)/test/libc/release/smoke.com.dbg:				\
		o/$(MODE)/test/libc/release/smoke.o			\
		o/$(MODE)/ape/public/ape.lds				\
		o/$(MODE)/libc/crt/crt.o				\
		o/$(MODE)/ape/ape.o					\
		o/$(MODE)/cosmopolitan.a
	@$(COMPILE) -ALD $(LD)						\
		-static							\
		-no-pie							\
		-nostdlib						\
		-T o/$(MODE)/ape/public/ape.lds				\
		o/$(MODE)/test/libc/release/smoke.o			\
		o/$(MODE)/libc/crt/crt.o				\
		o/$(MODE)/ape/ape.o					\
		o/$(MODE)/cosmopolitan.a				\
		-o $@

o/$(MODE)/test/libc/release/smoke-nms.com.dbg:				\
		o/$(MODE)/test/libc/release/smoke.o			\
		o/$(MODE)/ape/public/ape.lds				\
		o/$(MODE)/libc/crt/crt.o				\
		o/$(MODE)/ape/ape-no-modify-self.o			\
		o/$(MODE)/cosmopolitan.a
	@$(COMPILE) -ALD $(LD)						\
		-static							\
		-no-pie							\
		-nostdlib						\
		-T o/$(MODE)/ape/public/ape.lds				\
		o/$(MODE)/test/libc/release/smoke.o			\
		o/$(MODE)/libc/crt/crt.o				\
		o/$(MODE)/ape/ape-no-modify-self.o			\
		o/$(MODE)/cosmopolitan.a				\
		-o $@

o/$(MODE)/test/libc/release/smoke-chibicc.com.dbg:			\
		o/$(MODE)/test/libc/release/smoke-chibicc.o		\
		o/$(MODE)/ape/public/ape.lds				\
		o/$(MODE)/libc/crt/crt.o				\
		o/$(MODE)/ape/ape-no-modify-self.o			\
		o/$(MODE)/cosmopolitan.a				\
		o/$(MODE)/third_party/chibicc/chibicc.com
	@$(COMPILE) -ALD $(LD)						\
		-static							\
		-no-pie							\
		-nostdlib						\
		-T o/$(MODE)/ape/public/ape.lds				\
		o/$(MODE)/test/libc/release/smoke-chibicc.o		\
		o/$(MODE)/libc/crt/crt.o				\
		o/$(MODE)/ape/ape-no-modify-self.o			\
		o/$(MODE)/cosmopolitan.a				\
		-o $@

o/$(MODE)/test/libc/release/smoke-chibicc.o:				\
		test/libc/release/smoke.c				\
		o/cosmopolitan.h					\
		o/$(MODE)/third_party/chibicc/chibicc.com
	@$(COMPILE) -wACHIBICC						\
		o/$(MODE)/third_party/chibicc/chibicc.com		\
		$(CHIBICC_FLAGS)					\
		-o $@							\
		-c							\
		-Os							\
		-static							\
		-fno-pie						\
		-nostdlib						\
		-nostdinc						\
		-mno-red-zone						\
		-fno-omit-frame-pointer					\
		-include o/cosmopolitan.h				\
		$<

o/$(MODE)/test/libc/release/smokecxx.com.dbg:				\
		o/$(MODE)/test/libc/release/smokecxx.o			\
		o/$(MODE)/ape/public/ape.lds				\
		o/$(MODE)/libc/crt/crt.o				\
		o/$(MODE)/ape/ape.o					\
		o/$(MODE)/cosmopolitan.a
	@$(COMPILE) -ALD $(LD)						\
		-static							\
		-no-pie							\
		-nostdlib						\
		-T o/$(MODE)/ape/public/ape.lds				\
		o/$(MODE)/test/libc/release/smokecxx.o			\
		o/$(MODE)/libc/crt/crt.o				\
		o/$(MODE)/ape/ape.o					\
		o/$(MODE)/cosmopolitan.a				\
		-o $@

o/$(MODE)/test/libc/release/smokecxx.o:					\
		test/libc/release/smokecxx.cc				\
		o/cosmopolitan.h
	@$(COMPILE) -ACXX $(CXX)					\
		-o $@							\
		-c							\
		-Os							\
		-fno-pie						\
		-nostdinc						\
		-fno-omit-frame-pointer					\
		-include o/cosmopolitan.h				\
		test/libc/release/smokecxx.cc

o/$(MODE)/test/libc/release/smokeansi.com.dbg:				\
		o/$(MODE)/test/libc/release/smokeansi.o			\
		o/$(MODE)/ape/public/ape.lds				\
		o/$(MODE)/libc/crt/crt.o				\
		o/$(MODE)/ape/ape.o					\
		o/$(MODE)/cosmopolitan.a
	@$(COMPILE) -ALD $(LD)						\
		-static							\
		-no-pie							\
		-nostdlib						\
		-T o/$(MODE)/ape/public/ape.lds				\
		o/$(MODE)/test/libc/release/smokeansi.o			\
		o/$(MODE)/libc/crt/crt.o				\
		o/$(MODE)/ape/ape.o					\
		o/$(MODE)/cosmopolitan.a				\
		-o $@

o/$(MODE)/test/libc/release/smokeansi.o:				\
		test/libc/release/smoke.c				\
		o/cosmopolitan.h
	@$(COMPILE) -AANSI $(CC)					\
		-o $@							\
		-c							\
		-Os							\
		-ansi							\
		-static							\
		-fno-pie						\
		-nostdinc						\
		-fno-omit-frame-pointer					\
		-include o/cosmopolitan.h				\
		test/libc/release/smoke.c

# TODO(jart): Rewrite these shell scripts as C code.
# o/$(MODE)/test/libc/release/metal.ok:					\
# 		test/libc/release/metal.sh				\
# 		o/$(MODE)/examples/hello.com				\
# 		o/$(MODE)/tool/build/blinkenlights.com.dbg
# 	@$(COMPILE) -ASHTEST -tT$@ $<
# o/$(MODE)/test/libc/release/emulate.ok:				\
# 		test/libc/release/emulate.sh				\
# 		o/$(MODE)/examples/hello.com				\
# 		o/$(MODE)/tool/build/blinkenlights.com.dbg
# 	@$(COMPILE) -ASHTEST -tT$@ $<

.PHONY: o/$(MODE)/test/libc/release
o/$(MODE)/test/libc/release:						\
		o/$(MODE)/test/libc/release/smoke.com			\
		o/$(MODE)/test/libc/release/smoke.com.runs		\
		o/$(MODE)/test/libc/release/smoke-nms.com		\
		o/$(MODE)/test/libc/release/smoke-nms.com.runs		\
		o/$(MODE)/test/libc/release/smoke-chibicc.com		\
		o/$(MODE)/test/libc/release/smoke-chibicc.com.runs	\
		o/$(MODE)/test/libc/release/smokecxx.com		\
		o/$(MODE)/test/libc/release/smokecxx.com.runs		\
		o/$(MODE)/test/libc/release/smokeansi.com		\
		o/$(MODE)/test/libc/release/smokeansi.com.runs
