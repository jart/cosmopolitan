#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘
#
# OVERVIEW
#
#   αcτµαlly pδrταblε εxεcµταblε
#
# DESCRIPTION
#
#   This file defines the libraries, runtimes, and build rules needed to
#   create executables from your Linux workstation that'll run anywhere.
#   Loading this package will make certain systemic modifications to the
#   build like turning off the System V "Red Zone" optimization, because
#   αcτµαlly pδrταblε εxεcµταblεs need to be able to run in kernelspace.

PKGS += APE

APE_FILES := $(wildcard ape/*.*)
APE_HDRS = $(filter %.h,$(APE_FILES))
APE_INCS = $(filter %.inc,$(APE_FILES))

ifeq ($(ARCH), aarch64)

APE = o/$(MODE)/ape/aarch64.lds

APELINK =					\
	$(COMPILE)				\
	-ALINK.ape				\
	$(LINK)					\
	$(LINKARGS)				\
	$(OUTPUT_OPTION) &&			\
	$(COMPILE)				\
	-AFIXUP.ape				\
	-wT$@					\
	$(FIXUPOBJ)				\
	$@

APE_SRCS = ape/ape.S ape/start.S ape/launch.S ape/systemcall.S
APE_OBJS = o/$(MODE)/ape/ape.o
APE_NO_MODIFY_SELF = $(APE)
APE_COPY_SELF = $(APE)

.PHONY: o/$(MODE)/ape
o/$(MODE)/ape: $(APE)

o/$(MODE)/ape/aarch64.lds:			\
	ape/aarch64.lds				\
	libc/zip.internal.h			\
	libc/thread/tls.h			\
	libc/calls/struct/timespec.h		\
	libc/macros.internal.h			\
	libc/str/str.h

APE_LOADER_LDFLAGS =				\
	-pie					\
	-static					\
	-nostdlib				\
	--no-dynamic-linker			\
	-z norelro				\
	-z common-page-size=0x4000		\
	-z max-page-size=0x4000

APE_LOADER_FLAGS =				\
	-DNDEBUG				\
	-iquote.				\
	-Wall					\
	-Wextra					\
	-fpie					\
	-Os					\
	-ffreestanding				\
	-mgeneral-regs-only			\
	-fno-asynchronous-unwind-tables		\
	-fno-stack-protector			\
	-fno-ident				\
	-fno-gnu-unique				\
	-c					\
	 $(OUTPUT_OPTION)			\
	$<

o/$(MODE)/ape/ape.elf: o/$(MODE)/ape/ape.elf.dbg
	@$(COMPILE) -AOBJCOPY -T$@ $(OBJCOPY) -g $< $@
	@$(COMPILE) -AFIXUPOBJ -wT$@ $(FIXUPOBJ) $@

o/$(MODE)/ape/ape.elf.dbg:			\
		o/$(MODE)/ape/start.o		\
		o/$(MODE)/ape/loader.o		\
		o/$(MODE)/ape/launch.o		\
		o/$(MODE)/ape/systemcall.o
	@$(COMPILE) -ALINK.elf $(LD) $(APE_LOADER_LDFLAGS) -o $@ $(patsubst %.lds,-T %.lds,$^)

o/$(MODE)/ape/loader.o: ape/loader.c ape/ape.h
	@$(COMPILE) -AOBJECTIFY.c $(CC) -DSUPPORT_VECTOR=33 -g $(APE_LOADER_FLAGS)
o/$(MODE)/ape/start.o: ape/start.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/ape/launch.o: ape/launch.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/ape/systemcall.o: ape/systemcall.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<

.PHONY: o/$(MODE)/ape
o/$(MODE)/ape: o/$(MODE)/ape/ape.elf

else

APE =	o/$(MODE)/ape/ape.o			\
	o/$(MODE)/ape/ape.lds

APELINK =					\
	$(COMPILE)				\
	-ALINK.ape				\
	$(LINK)					\
	$(LINKARGS)				\
	$(OUTPUT_OPTION) &&			\
	$(COMPILE)				\
	-AFIXUP.ape				\
	-wT$@					\
	$(FIXUPOBJ)				\
	$@

APE_NO_MODIFY_SELF =				\
	o/$(MODE)/ape/ape.lds			\
	o/$(MODE)/ape/ape-no-modify-self.o

APE_COPY_SELF =					\
	o/$(MODE)/ape/ape.lds			\
	o/$(MODE)/ape/ape-copy-self.o

APE_LOADER_FLAGS =				\
	-DNDEBUG				\
	-iquote.				\
	-Wall					\
	-Wextra					\
	-Werror					\
	-pedantic-errors			\
	-fpie					\
	-Os					\
	-ffreestanding				\
	-mgeneral-regs-only			\
	-fno-stack-protector			\
	-fno-ident				\
	-fno-gnu-unique				\
	-c					\
	 $(OUTPUT_OPTION)			\
	$<

APE_SRCS_C = ape/loader.c
APE_SRCS_S = $(filter %.S,$(APE_FILES))
APE_SRCS = $(APE_SRCS_C) $(APE_SRCS_S)
APE_OBJS = $(APE_SRCS_S:%.S=o/$(MODE)/%.o)
APE_CHECKS = $(APE_HDRS:%=o/%.ok)

o/ape/idata.inc:				\
		ape/idata.internal.h		\
		ape/relocations.h

o/$(MODE)/ape/ape-no-modify-self.o:		\
		ape/ape.S			\
		ape/ape.h			\
		ape/macros.internal.h		\
		ape/notice.inc			\
		ape/relocations.h		\
		ape/ape.internal.h		\
		libc/dce.h			\
		libc/elf/def.h			\
		libc/thread/tls.h		\
		libc/intrin/asancodes.h		\
		libc/macho.internal.h		\
		libc/macros.internal.h		\
		libc/nexgen32e/uart.internal.h	\
		libc/calls/metalfile.internal.h	\
		libc/nt/pedef.internal.h	\
		libc/runtime/e820.internal.h	\
		libc/runtime/mman.internal.h	\
		libc/runtime/pc.internal.h	\
		libc/sysv/consts/prot.h		\
		o/$(MODE)/ape/ape.elf
	@$(COMPILE)				\
		-AOBJECTIFY.S			\
		$(OBJECTIFY.S)			\
		$(OUTPUT_OPTION)		\
		-DAPE_NO_MODIFY_SELF		\
		-DAPE_LOADER='"o/$(MODE)/ape/ape.elf"' $<

o/$(MODE)/ape/ape-copy-self.o:			\
		ape/ape.S			\
		ape/ape.h			\
		ape/macros.internal.h		\
		ape/notice.inc			\
		ape/relocations.h		\
		ape/ape.internal.h		\
		libc/dce.h			\
		libc/elf/def.h			\
		libc/thread/tls.h		\
		libc/intrin/asancodes.h		\
		libc/macho.internal.h		\
		libc/macros.internal.h		\
		libc/nexgen32e/uart.internal.h	\
		libc/calls/metalfile.internal.h	\
		libc/nt/pedef.internal.h	\
		libc/runtime/e820.internal.h	\
		libc/runtime/mman.internal.h	\
		libc/runtime/pc.internal.h	\
		libc/sysv/consts/prot.h
	@$(COMPILE)				\
		-AOBJECTIFY.S			\
		$(OBJECTIFY.S)			\
		$(OUTPUT_OPTION)		\
		-DAPE_NO_MODIFY_SELF $<

o/$(MODE)/ape/loader.o: ape/loader.c
	@$(COMPILE) -AOBJECTIFY.c $(CC) -DSUPPORT_VECTOR=121 -g $(APE_LOADER_FLAGS)
o/$(MODE)/ape/loader-gcc.asm: ape/loader.c
	@$(COMPILE) -AOBJECTIFY.c $(CC) -DSUPPORT_VECTOR=121 -S -g0 $(APE_LOADER_FLAGS)
o/$(MODE)/ape/loader-clang.asm: ape/loader.c
	@$(COMPILE) -AOBJECTIFY.c $(CLANG) -DSUPPORT_VECTOR=121 -S -g0 $(APE_LOADER_FLAGS)

o/$(MODE)/ape/loader-xnu.o: ape/loader.c
	@$(COMPILE) -AOBJECTIFY.c $(CC) -DSUPPORT_VECTOR=8 -g $(APE_LOADER_FLAGS)
o/$(MODE)/ape/loader-xnu-gcc.asm: ape/loader.c
	@$(COMPILE) -AOBJECTIFY.c $(CC) -DSUPPORT_VECTOR=8 -S -g0 $(APE_LOADER_FLAGS)
o/$(MODE)/ape/loader-xnu-clang.asm: ape/loader.c
	@$(COMPILE) -AOBJECTIFY.c $(CLANG) -DSUPPORT_VECTOR=8 -S -g0 $(APE_LOADER_FLAGS)

o/$(MODE)/ape/ape.elf: o/$(MODE)/ape/ape.elf.dbg
	@$(COMPILE) -AOBJBINCOPY -w build/bootstrap/objbincopy.com -f -o $@ $<

o/$(MODE)/ape/ape.macho: o/$(MODE)/ape/ape.elf.dbg
	@$(COMPILE) -AOBJBINCOPY -w build/bootstrap/objbincopy.com -fm -o $@ $<

APE_LOADER_LDFLAGS =				\
	-static					\
	-nostdlib				\
	--no-dynamic-linker			\
	-z separate-code			\
	-z common-page-size=0x1000		\
	-z max-page-size=0x10000

o/$(MODE)/ape/ape.elf.dbg:			\
		o/$(MODE)/ape/loader-macho.o	\
		o/$(MODE)/ape/start.o		\
		o/$(MODE)/ape/loader.o		\
		o/$(MODE)/ape/launch.o		\
		o/$(MODE)/ape/systemcall.o	\
		ape/loader.lds
	@$(COMPILE) -ALINK.elf $(LD) $(APE_LOADER_LDFLAGS) -o $@ $(patsubst %.lds,-T %.lds,$^)

.PHONY: o/$(MODE)/ape
o/$(MODE)/ape:	$(APE_CHECKS)			\
		o/$(MODE)/ape/ape.o		\
		o/$(MODE)/ape/ape.lds		\
		o/$(MODE)/ape/ape.elf		\
		o/$(MODE)/ape/ape.macho		\
		o/$(MODE)/ape/ape-copy-self.o	\
		o/$(MODE)/ape/ape-no-modify-self.o

endif

# these assembly files are safe to build on aarch64
o/$(MODE)/ape/ape.o: ape/ape.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<

o/$(MODE)/ape/ape.lds:				\
		ape/ape.lds			\
		ape/macros.internal.h		\
		ape/relocations.h		\
		ape/ape.internal.h		\
		libc/thread/tls.h		\
		libc/calls/struct/timespec.h	\
		libc/thread/thread.h		\
		libc/dce.h			\
		libc/elf/def.h			\
		libc/elf/pf2prot.internal.h	\
		libc/macros.internal.h		\
		libc/nt/pedef.internal.h	\
		libc/str/str.h			\
		libc/zip.internal.h
