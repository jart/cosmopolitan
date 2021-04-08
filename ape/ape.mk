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

APE =	$(APE_DEPS)			\
	$(APE_OBJS)			\
	o/$(MODE)/ape/ape.lds

APELINK =				\
	$(COMPILE)			\
	-ALINK.ape			\
	$(LINK)				\
	$(LINKARGS)			\
	$(OUTPUT_OPTION)

APE_FILES := $(wildcard ape/*.*)
APE_HDRS = $(filter %.h,$(APE_FILES))
APE_INCS = $(filter %.inc,$(APE_FILES))
APE_SRCS = $(filter %.S,$(APE_FILES))
APE_OBJS = $(APE_SRCS:%.S=o/$(MODE)/%.o)
APE_CHECKS = $(APE_HDRS:%=o/%.ok)

o/$(MODE)/ape/ape.lds:			\
		ape/ape.lds		\
		ape/macros.internal.h	\
		libc/dce.h		\
		libc/zip.h

o/ape/idata.inc:			\
		ape/idata.internal.h	\
		ape/relocations.h

$(APE_OBJS):	$(BUILD_FILES)		\
		ape/ape.mk

o/$(MODE)/ape/ape-no-modify-self.o: ape/ape.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -DAPE_NO_MODIFY_SELF $<

.PHONY: o/$(MODE)/ape
o/$(MODE)/ape:	$(APE)			\
		$(APE_CHECKS)		\
		o/$(MODE)/ape/ape-no-modify-self.o
