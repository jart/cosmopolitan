#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += LIBC_NEXGEN32E

LIBC_NEXGEN32E_ARTIFACTS += LIBC_NEXGEN32E_A
LIBC_NEXGEN32E = $(LIBC_NEXGEN32E_A_DEPS) $(LIBC_NEXGEN32E_A)
LIBC_NEXGEN32E_A = o/$(MODE)/libc/nexgen32e/nexgen32e.a
LIBC_NEXGEN32E_A_FILES := $(wildcard libc/nexgen32e/*)
LIBC_NEXGEN32E_A_HDRS = $(filter %.h,$(LIBC_NEXGEN32E_A_FILES))
LIBC_NEXGEN32E_A_INCS = $(filter %.inc,$(LIBC_NEXGEN32E_A_FILES))
LIBC_NEXGEN32E_A_SRCS_A = $(filter %.s,$(LIBC_NEXGEN32E_A_FILES))
LIBC_NEXGEN32E_A_SRCS_S = $(filter %.S,$(LIBC_NEXGEN32E_A_FILES))
LIBC_NEXGEN32E_A_SRCS_C = $(filter %.c,$(LIBC_NEXGEN32E_A_FILES))

LIBC_NEXGEN32E_A_SRCS =					\
	$(LIBC_NEXGEN32E_A_SRCS_A)			\
	$(LIBC_NEXGEN32E_A_SRCS_S)			\
	$(LIBC_NEXGEN32E_A_SRCS_C)

LIBC_NEXGEN32E_A_OBJS =					\
	$(LIBC_NEXGEN32E_A_SRCS_A:%.s=o/$(MODE)/%.o)	\
	$(LIBC_NEXGEN32E_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(LIBC_NEXGEN32E_A_SRCS_C:%.c=o/$(MODE)/%.o)

LIBC_NEXGEN32E_A_CHECKS =				\
	$(LIBC_NEXGEN32E_A).pkg				\
	$(LIBC_NEXGEN32E_A_HDRS:%=o/$(MODE)/%.ok)

LIBC_NEXGEN32E_A_DEPS :=				\
	$(call uniq,$(foreach x,$(LIBC_NEXGEN32E_A_DIRECTDEPS),$($(x))))

$(LIBC_NEXGEN32E_A):					\
		libc/nexgen32e/				\
		$(LIBC_NEXGEN32E_A).pkg			\
		$(LIBC_NEXGEN32E_A_OBJS)

$(LIBC_NEXGEN32E_A).pkg:				\
		$(LIBC_NEXGEN32E_A_OBJS)		\
		$(foreach x,$(LIBC_NEXGEN32E_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/libc/nexgen32e/envp.o				\
o/$(MODE)/libc/nexgen32e/argc2.o			\
o/$(MODE)/libc/nexgen32e/argv2.o			\
o/$(MODE)/libc/nexgen32e/auxv2.o			\
o/$(MODE)/libc/nexgen32e/cescapec.o			\
o/$(MODE)/libc/nexgen32e/crc32init.o			\
o/$(MODE)/libc/nexgen32e/environ2.o			\
o/$(MODE)/libc/nexgen32e/kbase36.o			\
o/$(MODE)/libc/nexgen32e/ktens.o			\
o/$(MODE)/libc/nexgen32e/ktolower.o			\
o/$(MODE)/libc/nexgen32e/ktoupper.o			\
o/$(MODE)/libc/nexgen32e/runlevel.o			\
o/$(MODE)/libc/nexgen32e/pid.o				\
o/$(MODE)/libc/nexgen32e/program_executable_name.o	\
o/$(MODE)/libc/nexgen32e/program_invocation_name2.o	\
o/$(MODE)/libc/nexgen32e/threaded.o: private		\
		CFLAGS +=				\
			$(NO_MAGIC)

# these assembly files are safe to build on aarch64
o/$(MODE)/libc/nexgen32e/gc.o: libc/nexgen32e/gc.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/nexgen32e/zip.o: libc/nexgen32e/zip.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/nexgen32e/mcount.o: libc/nexgen32e/mcount.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/nexgen32e/ksha256.o: libc/nexgen32e/ksha256.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/nexgen32e/ksha512.o: libc/nexgen32e/ksha512.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/nexgen32e/kcp437.o: libc/nexgen32e/kcp437.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/nexgen32e/kreversebits.o: libc/nexgen32e/kreversebits.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/nexgen32e/ktensindex.o: libc/nexgen32e/ktensindex.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/nexgen32e/longjmp.o: libc/nexgen32e/longjmp.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/nexgen32e/setjmp.o: libc/nexgen32e/setjmp.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/nexgen32e/missingno.o: libc/nexgen32e/missingno.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/nexgen32e/khalfcache3.o: libc/nexgen32e/khalfcache3.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/nexgen32e/gclongjmp.o: libc/nexgen32e/gclongjmp.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/nexgen32e/checkstackalign.o: libc/nexgen32e/checkstackalign.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/nexgen32e/blink_xnu_aarch64.o: libc/nexgen32e/blink_xnu_aarch64.S ape/blink-xnu-aarch64.gz
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/nexgen32e/blink_linux_aarch64.o: libc/nexgen32e/blink_linux_aarch64.S ape/blink-linux-aarch64.gz
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<

LIBC_NEXGEN32E_LIBS = $(foreach x,$(LIBC_NEXGEN32E_ARTIFACTS),$($(x)))
LIBC_NEXGEN32E_SRCS = $(foreach x,$(LIBC_NEXGEN32E_ARTIFACTS),$($(x)_SRCS))
LIBC_NEXGEN32E_HDRS = $(foreach x,$(LIBC_NEXGEN32E_ARTIFACTS),$($(x)_HDRS))
LIBC_NEXGEN32E_INCS = $(foreach x,$(LIBC_NEXGEN32E_ARTIFACTS),$($(x)_INCS))
LIBC_NEXGEN32E_CHECKS = $(foreach x,$(LIBC_NEXGEN32E_ARTIFACTS),$($(x)_CHECKS))
LIBC_NEXGEN32E_OBJS = $(foreach x,$(LIBC_NEXGEN32E_ARTIFACTS),$($(x)_OBJS))
$(LIBC_NEXGEN32E_OBJS): $(BUILD_FILES) libc/nexgen32e/BUILD.mk

.PHONY: o/$(MODE)/libc/nexgen32e
o/$(MODE)/libc/nexgen32e: $(LIBC_NEXGEN32E_CHECKS)
