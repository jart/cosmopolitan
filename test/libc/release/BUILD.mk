#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

ifneq ($(MODE), dbg)
ifneq ($(MODE), asan)
ifeq ($(ARCH), x86_64)

o/$(MODE)/test/libc/release/cosmopolitan.zip: private .UNSANDBOXED = 1
o/$(MODE)/test/libc/release/cosmopolitan.zip:			\
		o/cosmopolitan.h				\
		o/$(MODE)/ape/ape.lds				\
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
		o/$(MODE)/ape/ape.lds				\
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
		-Werror							\
		-fno-pie						\
		-nostdinc						\
		-D_COSMO_SOURCE						\
		-Wl,--gc-sections					\
		-fno-omit-frame-pointer					\
		-include o/cosmopolitan.h				\
		-Wl,-z,max-page-size=0x1000				\
		-Wl,-z,common-page-size=0x1000				\
		$<

o/$(MODE)/test/libc/release/smoke.com.dbg:				\
		o/$(MODE)/test/libc/release/smoke.o			\
		o/$(MODE)/ape/ape.lds					\
		o/$(MODE)/libc/crt/crt.o				\
		o/$(MODE)/ape/ape.o					\
		o/$(MODE)/cosmopolitan.a
	@$(COMPILE) -ALD $(LD)						\
		-static							\
		-no-pie							\
		-nostdlib						\
		--gc-sections						\
		-z max-page-size=0x1000					\
		-z common-page-size=0x1000				\
		-T o/$(MODE)/ape/ape.lds				\
		o/$(MODE)/test/libc/release/smoke.o			\
		o/$(MODE)/libc/crt/crt.o				\
		o/$(MODE)/ape/ape.o					\
		o/$(MODE)/cosmopolitan.a				\
		-o $@

o/$(MODE)/test/libc/release/smoke-nms.com.dbg:				\
		o/$(MODE)/test/libc/release/smoke.o			\
		o/$(MODE)/ape/ape.lds					\
		o/$(MODE)/libc/crt/crt.o				\
		o/$(MODE)/ape/ape-no-modify-self.o			\
		o/$(MODE)/cosmopolitan.a
	@$(COMPILE) -ALD $(LD)						\
		-static							\
		-no-pie							\
		-nostdlib						\
		--gc-sections						\
		-z max-page-size=0x1000					\
		-z common-page-size=0x1000				\
		-T o/$(MODE)/ape/ape.lds				\
		o/$(MODE)/test/libc/release/smoke.o			\
		o/$(MODE)/libc/crt/crt.o				\
		o/$(MODE)/ape/ape-no-modify-self.o			\
		o/$(MODE)/cosmopolitan.a				\
		-o $@

o/$(MODE)/test/libc/release/smoke-chibicc.com.dbg:			\
		o/$(MODE)/test/libc/release/smoke-chibicc.o		\
		o/$(MODE)/ape/ape.lds					\
		o/$(MODE)/libc/crt/crt.o				\
		o/$(MODE)/ape/ape-no-modify-self.o			\
		o/$(MODE)/cosmopolitan.a				\
		o/$(MODE)/third_party/chibicc/chibicc.com
	@$(COMPILE) -ALD $(LD)						\
		-static							\
		-no-pie							\
		-nostdlib						\
		--gc-sections						\
		-z max-page-size=0x1000					\
		-z common-page-size=0x1000				\
		-T o/$(MODE)/ape/ape.lds				\
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
		-Werror							\
		-fno-pie						\
		-nostdlib						\
		-nostdinc						\
		-mno-red-zone						\
		-D_COSMO_SOURCE						\
		-fno-omit-frame-pointer					\
		-include o/cosmopolitan.h				\
		$<

o/$(MODE)/test/libc/release/smokecxx.com.dbg:				\
		o/$(MODE)/test/libc/release/smokecxx.o			\
		o/$(MODE)/ape/ape.lds					\
		o/$(MODE)/libc/crt/crt.o				\
		o/$(MODE)/ape/ape.o					\
		o/$(MODE)/cosmopolitan.a				\
		o/$(MODE)/third_party/libcxx/libcxx.a
	@$(COMPILE) -ALD $(LD)						\
		-static							\
		-no-pie							\
		-nostdlib						\
		--gc-sections						\
		-z max-page-size=0x1000					\
		-z common-page-size=0x1000				\
		-T o/$(MODE)/ape/ape.lds				\
		o/$(MODE)/test/libc/release/smokecxx.o			\
		o/$(MODE)/libc/crt/crt.o				\
		o/$(MODE)/ape/ape.o					\
		o/$(MODE)/third_party/libcxx/libcxx.a			\
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
		-Wl,--gc-sections					\
		-fno-omit-frame-pointer					\
		-z max-page-size=0x1000					\
		-z common-page-size=0x1000				\
		-include o/cosmopolitan.h				\
		test/libc/release/smokecxx.cc

o/$(MODE)/test/libc/release/smokeansi.com.dbg:				\
		o/$(MODE)/test/libc/release/smokeansi.o			\
		o/$(MODE)/ape/ape.lds					\
		o/$(MODE)/libc/crt/crt.o				\
		o/$(MODE)/ape/ape.o					\
		o/$(MODE)/cosmopolitan.a
	@$(COMPILE) -ALD $(LD)						\
		-static							\
		-no-pie							\
		-nostdlib						\
		--gc-sections						\
		-z max-page-size=0x1000					\
		-z common-page-size=0x1000				\
		-T o/$(MODE)/ape/ape.lds				\
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
		-Werror							\
		-static							\
		-fno-pie						\
		-nostdinc						\
		-D_COSMO_SOURCE						\
		-Wl,--gc-sections					\
		-fno-omit-frame-pointer					\
		-include o/cosmopolitan.h				\
		-Wl,-z,max-page-size=0x1000				\
		-Wl,-z,common-page-size=0x1000				\
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

endif
endif
endif

.PHONY: o/$(MODE)/test/libc/release
o/$(MODE)/test/libc/release:
