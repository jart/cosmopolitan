#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘
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

APE =	o/$(MODE)/ape/ape.o		\
	o/$(MODE)/ape/ape.lds

APE_NO_MODIFY_SELF =			\
	o/$(MODE)/ape/ape.lds		\
	o/$(MODE)/ape/ape-no-modify-self.o

APELINK =				\
	$(COMPILE)			\
	-ALINK.ape			\
	$(LINK)				\
	$(LINKARGS)			\
	$(OUTPUT_OPTION)

APE_FILES := $(wildcard ape/*.*)
APE_HDRS = $(filter %.h,$(APE_FILES))
APE_INCS = $(filter %.inc,$(APE_FILES))
APE_SRCS_C = ape/loader.c
APE_SRCS_S = $(filter %.S,$(APE_FILES))
APE_SRCS = $(APE_SRCS_C) $(APE_SRCS_S)
APE_OBJS = $(APE_SRCS_S:%.S=o/$(MODE)/%.o)
APE_CHECKS = $(APE_HDRS:%=o/%.ok)

o/$(MODE)/ape/ape.lds:			\
		ape/ape.lds		\
		ape/macros.internal.h	\
		libc/dce.h		\
		libc/zip.h

o/ape/idata.inc:			\
		ape/idata.internal.h	\
		ape/relocations.h

o/$(MODE)/ape/ape-no-modify-self.o: ape/ape.S o/$(MODE)/ape/loader.elf
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -DAPE_LOADER="\"o/$(MODE)/ape/loader.elf\"" -DAPE_NO_MODIFY_SELF $<

o/$(MODE)/ape/loader.o: ape/loader.c
	@$(COMPILE) -AOBJECTIFY.c $(CC) $(cpp.flags) -fpie -Os -ffreestanding -mno-red-zone -fno-ident -fno-gnu-unique -c $(OUTPUT_OPTION) $<

o/$(MODE)/ape/loader-gcc.asm: ape/loader.c
	@$(COMPILE) -AOBJECTIFY.c $(CC) $(cpp.flags) -Os -ffreestanding -mno-red-zone -fno-ident -fno-gnu-unique -c -S $(OUTPUT_OPTION) $<

o/$(MODE)/ape/loader.elf:		\
		o/$(MODE)/ape/loader.o	\
		o/$(MODE)/ape/loader1.o	\
		ape/loader.lds
	@$(ELFLINK) -s -z max-page-size=0x10

.PHONY: o/$(MODE)/ape
o/$(MODE)/ape:	$(APE)			\
		$(APE_CHECKS)		\
		o/$(MODE)/ape/ape-no-modify-self.o
