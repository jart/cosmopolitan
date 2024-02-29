#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += LIBC_NT

LIBC_NT_LIBS = $(foreach x,$(LIBC_NT_ARTIFACTS),$($(x)))
LIBC_NT_HDRS = $(foreach x,$(LIBC_NT_ARTIFACTS),$($(x)_HDRS))
LIBC_NT_INCS = $(foreach x,$(LIBC_NT_ARTIFACTS),$($(x)_INCS))
LIBC_NT_SRCS = $(foreach x,$(LIBC_NT_ARTIFACTS),$($(x)_SRCS))
LIBC_NT_OBJS = $(foreach x,$(LIBC_NT_ARTIFACTS),$($(x)_OBJS))
LIBC_NT_CHECKS = $(foreach x,$(LIBC_NT_ARTIFACTS),$($(x)_CHECKS))

#───────────────────────────────────────────────────────────────────────────────

LIBC_NT_ARTIFACTS += LIBC_NT_A

LIBC_NT_A_FILES :=					\
	$(wildcard libc/nt/enum/*)			\
	$(wildcard libc/nt/struct/*)			\
	$(wildcard libc/nt/typedef/*)			\
	$(wildcard libc/nt/thunk/*)			\
	$(wildcard libc/nt/nt/thunk/*)			\
	$(wildcard libc/nt/nt/*.*)			\
	$(wildcard libc/nt/*)

LIBC_NT_A_HDRS = $(filter %.h,$(LIBC_NT_A_FILES))
LIBC_NT_A_INCS = $(filter %.inc,$(LIBC_NT_A_FILES))
LIBC_NT_A_CHECKS = $(patsubst %,o/$(MODE)/%.ok,$(filter %.h,$(LIBC_NT_A_HDRS)))

#───────────────────────────────────────────────────────────────────────────────

LIBC_NT_ARTIFACTS += LIBC_NT_KERNEL32_A
LIBC_NT_KERNEL32 = $(LIBC_NT_KERNEL32_A_DEPS) $(LIBC_NT_KERNEL32_A)
LIBC_NT_KERNEL32_A = o/$(MODE)/libc/nt/kernel32.a
LIBC_NT_KERNEL32_A_SRCS := $(wildcard libc/nt/kernel32/*.S) libc/nt/sysv2nt.S
LIBC_NT_KERNEL32_A_OBJS = $(LIBC_NT_KERNEL32_A_SRCS:%.S=o/$(MODE)/%.o)
LIBC_NT_KERNEL32_A_CHECKS = $(LIBC_NT_KERNEL32_A).pkg
LIBC_NT_KERNEL32_A_DEPS :=				\
	$(call uniq,$(foreach x,$(LIBC_NT_KERNEL32_A_DIRECTDEPS),$($(x))))

$(LIBC_NT_KERNEL32_A):					\
		libc/nt/kernel32/			\
		$(LIBC_NT_KERNEL32_A).pkg		\
		$(LIBC_NT_KERNEL32_A_OBJS)

$(LIBC_NT_KERNEL32_A).pkg:				\
		$(LIBC_NT_KERNEL32_A_OBJS)		\
		$(foreach x,$(LIBC_NT_KERNEL32_A_DIRECTDEPS),$($(x)_A).pkg)

#───────────────────────────────────────────────────────────────────────────────

LIBC_NT_ARTIFACTS += LIBC_NT_ADVAPI32_A
LIBC_NT_ADVAPI32 = $(LIBC_NT_ADVAPI32_A_DEPS) $(LIBC_NT_ADVAPI32_A)
LIBC_NT_ADVAPI32_A = o/$(MODE)/libc/nt/advapi32.a
LIBC_NT_ADVAPI32_A_SRCS := $(wildcard libc/nt/advapi32/*.S)
LIBC_NT_ADVAPI32_A_OBJS = $(LIBC_NT_ADVAPI32_A_SRCS:%.S=o/$(MODE)/%.o)
LIBC_NT_ADVAPI32_A_CHECKS = $(LIBC_NT_ADVAPI32_A).pkg
LIBC_NT_ADVAPI32_A_DIRECTDEPS = LIBC_NT_KERNEL32
LIBC_NT_ADVAPI32_A_DEPS :=				\
	$(call uniq,$(foreach x,$(LIBC_NT_ADVAPI32_A_DIRECTDEPS),$($(x))))

$(LIBC_NT_ADVAPI32_A):					\
		libc/nt/advapi32/			\
		$(LIBC_NT_ADVAPI32_A).pkg		\
		$(LIBC_NT_ADVAPI32_A_OBJS)

$(LIBC_NT_ADVAPI32_A).pkg:				\
		$(LIBC_NT_ADVAPI32_A_OBJS)		\
		$(foreach x,$(LIBC_NT_ADVAPI32_A_DIRECTDEPS),$($(x)_A).pkg)

#───────────────────────────────────────────────────────────────────────────────

LIBC_NT_ARTIFACTS += LIBC_NT_COMDLG32_A
LIBC_NT_COMDLG32 = $(LIBC_NT_COMDLG32_A_DEPS) $(LIBC_NT_COMDLG32_A)
LIBC_NT_COMDLG32_A = o/$(MODE)/libc/nt/comdlg32.a
LIBC_NT_COMDLG32_A_SRCS := $(wildcard libc/nt/comdlg32/*.S)
LIBC_NT_COMDLG32_A_OBJS = $(LIBC_NT_COMDLG32_A_SRCS:%.S=o/$(MODE)/%.o)
LIBC_NT_COMDLG32_A_CHECKS = $(LIBC_NT_COMDLG32_A).pkg
LIBC_NT_COMDLG32_A_DIRECTDEPS = LIBC_NT_KERNEL32
LIBC_NT_COMDLG32_A_DEPS :=				\
	$(call uniq,$(foreach x,$(LIBC_NT_COMDLG32_A_DIRECTDEPS),$($(x))))

$(LIBC_NT_COMDLG32_A):					\
		libc/nt/comdlg32/			\
		$(LIBC_NT_COMDLG32_A).pkg		\
		$(LIBC_NT_COMDLG32_A_OBJS)

$(LIBC_NT_COMDLG32_A).pkg:				\
		$(LIBC_NT_COMDLG32_A_OBJS)		\
		$(foreach x,$(LIBC_NT_COMDLG32_A_DIRECTDEPS),$($(x)_A).pkg)

#───────────────────────────────────────────────────────────────────────────────

LIBC_NT_ARTIFACTS += LIBC_NT_GDI32_A
LIBC_NT_GDI32 = $(LIBC_NT_GDI32_A_DEPS) $(LIBC_NT_GDI32_A)
LIBC_NT_GDI32_A = o/$(MODE)/libc/nt/gdi32.a
LIBC_NT_GDI32_A_SRCS := $(wildcard libc/nt/gdi32/*.S)
LIBC_NT_GDI32_A_OBJS = $(LIBC_NT_GDI32_A_SRCS:%.S=o/$(MODE)/%.o)
LIBC_NT_GDI32_A_CHECKS = $(LIBC_NT_GDI32_A).pkg
LIBC_NT_GDI32_A_DIRECTDEPS = LIBC_NT_KERNEL32
LIBC_NT_GDI32_A_DEPS :=					\
	$(call uniq,$(foreach x,$(LIBC_NT_GDI32_A_DIRECTDEPS),$($(x))))

$(LIBC_NT_GDI32_A):					\
		libc/nt/gdi32/				\
		$(LIBC_NT_GDI32_A).pkg			\
		$(LIBC_NT_GDI32_A_OBJS)

$(LIBC_NT_GDI32_A).pkg:					\
		$(LIBC_NT_GDI32_A_OBJS)			\
		$(foreach x,$(LIBC_NT_GDI32_A_DIRECTDEPS),$($(x)_A).pkg)

#───────────────────────────────────────────────────────────────────────────────

LIBC_NT_ARTIFACTS += LIBC_NT_NTDLL_A
LIBC_NT_NTDLL = $(LIBC_NT_NTDLL_A_DEPS) $(LIBC_NT_NTDLL_A)
LIBC_NT_NTDLL_A = o/$(MODE)/libc/nt/ntdll.a
LIBC_NT_NTDLL_A_SRCS_A := $(wildcard libc/nt/ntdll/*.S)
LIBC_NT_NTDLL_A_SRCS_S = libc/nt/ntdllimport.S
LIBC_NT_NTDLL_A_SRCS = $(LIBC_NT_NTDLL_A_SRCS_A) $(LIBC_NT_NTDLL_A_SRCS_S)
LIBC_NT_NTDLL_A_OBJS =					\
	$(LIBC_NT_NTDLL_A_SRCS_A:%.S=o/$(MODE)/%.o)	\
	$(LIBC_NT_NTDLL_A_SRCS_S:%.S=o/$(MODE)/%.o)
LIBC_NT_NTDLL_A_CHECKS = $(LIBC_NT_NTDLL_A).pkg
LIBC_NT_NTDLL_A_DIRECTDEPS = LIBC_NT_KERNEL32
LIBC_NT_NTDLL_A_DEPS :=					\
	$(call uniq,$(foreach x,$(LIBC_NT_NTDLL_A_DIRECTDEPS),$($(x))))

$(LIBC_NT_NTDLL_A):					\
		libc/nt/ntdll/				\
		$(LIBC_NT_NTDLL_A).pkg			\
		$(LIBC_NT_NTDLL_A_OBJS)

$(LIBC_NT_NTDLL_A).pkg:					\
		$(LIBC_NT_NTDLL_A_OBJS)			\
		$(foreach x,$(LIBC_NT_NTDLL_A_DIRECTDEPS),$($(x)_A).pkg)

#───────────────────────────────────────────────────────────────────────────────

LIBC_NT_ARTIFACTS += LIBC_NT_SYNCHRONIZATION_A
LIBC_NT_SYNCHRONIZATION = $(LIBC_NT_SYNCHRONIZATION_A_DEPS) $(LIBC_NT_SYNCHRONIZATION_A)
LIBC_NT_SYNCHRONIZATION_A = o/$(MODE)/libc/nt/synchronization.a
LIBC_NT_SYNCHRONIZATION_A_SRCS := $(wildcard libc/nt/API-MS-Win-Core-Synch-l1-2-0/*.S)
LIBC_NT_SYNCHRONIZATION_A_OBJS = $(LIBC_NT_SYNCHRONIZATION_A_SRCS:%.S=o/$(MODE)/%.o)
LIBC_NT_SYNCHRONIZATION_A_CHECKS = $(LIBC_NT_SYNCHRONIZATION_A).pkg
LIBC_NT_SYNCHRONIZATION_A_DIRECTDEPS = LIBC_NT_KERNEL32
LIBC_NT_SYNCHRONIZATION_A_DEPS :=				\
	$(call uniq,$(foreach x,$(LIBC_NT_SYNCHRONIZATION_A_DIRECTDEPS),$($(x))))

$(LIBC_NT_SYNCHRONIZATION_A):					\
		libc/nt/API-MS-Win-Core-Synch-l1-2-0/		\
		$(LIBC_NT_SYNCHRONIZATION_A).pkg		\
		$(LIBC_NT_SYNCHRONIZATION_A_OBJS)

$(LIBC_NT_SYNCHRONIZATION_A).pkg:				\
		$(LIBC_NT_SYNCHRONIZATION_A_OBJS)		\
		$(foreach x,$(LIBC_NT_SYNCHRONIZATION_A_DIRECTDEPS),$($(x)_A).pkg)

#───────────────────────────────────────────────────────────────────────────────

LIBC_NT_ARTIFACTS += LIBC_NT_USER32_A
LIBC_NT_USER32 = $(LIBC_NT_USER32_A_DEPS) $(LIBC_NT_USER32_A)
LIBC_NT_USER32_A = o/$(MODE)/libc/nt/user32.a
LIBC_NT_USER32_A_SRCS := $(wildcard libc/nt/user32/*.S)
LIBC_NT_USER32_A_OBJS = $(LIBC_NT_USER32_A_SRCS:%.S=o/$(MODE)/%.o)
LIBC_NT_USER32_A_CHECKS = $(LIBC_NT_USER32_A).pkg
LIBC_NT_USER32_A_DIRECTDEPS = LIBC_NT_KERNEL32
LIBC_NT_USER32_A_DEPS :=				\
	$(call uniq,$(foreach x,$(LIBC_NT_USER32_A_DIRECTDEPS),$($(x))))

$(LIBC_NT_USER32_A):					\
		libc/nt/user32/				\
		$(LIBC_NT_USER32_A).pkg			\
		$(LIBC_NT_USER32_A_OBJS)

$(LIBC_NT_USER32_A).pkg:				\
		$(LIBC_NT_USER32_A_OBJS)		\
		$(foreach x,$(LIBC_NT_USER32_A_DIRECTDEPS),$($(x)_A).pkg)

#───────────────────────────────────────────────────────────────────────────────

LIBC_NT_ARTIFACTS += LIBC_NT_WS2_32_A
LIBC_NT_WS2_32 = $(LIBC_NT_WS2_32_A_DEPS) $(LIBC_NT_WS2_32_A)
LIBC_NT_WS2_32_A = o/$(MODE)/libc/nt/ws2_32.a
LIBC_NT_WS2_32_A_SRCS := $(wildcard libc/nt/ws2_32/*.S)
LIBC_NT_WS2_32_A_OBJS = $(LIBC_NT_WS2_32_A_SRCS:%.S=o/$(MODE)/%.o)
LIBC_NT_WS2_32_A_CHECKS = $(LIBC_NT_WS2_32_A).pkg
LIBC_NT_WS2_32_A_DIRECTDEPS = LIBC_NT_KERNEL32
LIBC_NT_WS2_32_A_DEPS :=				\
	$(call uniq,$(foreach x,$(LIBC_NT_WS2_32_A_DIRECTDEPS),$($(x))))

$(LIBC_NT_WS2_32_A):					\
		libc/nt/ws2_32/				\
		$(LIBC_NT_WS2_32_A).pkg			\
		$(LIBC_NT_WS2_32_A_OBJS)

$(LIBC_NT_WS2_32_A).pkg:				\
		$(LIBC_NT_WS2_32_A_OBJS)		\
		$(foreach x,$(LIBC_NT_WS2_32_A_DIRECTDEPS),$($(x)_A).pkg)

#───────────────────────────────────────────────────────────────────────────────

LIBC_NT_ARTIFACTS += LIBC_NT_IPHLPAPI_A
LIBC_NT_IPHLPAPI = $(LIBC_NT_IPHLPAPI_A_DEPS) $(LIBC_NT_IPHLPAPI_A)
LIBC_NT_IPHLPAPI_A = o/$(MODE)/libc/nt/iphlpapi.a
LIBC_NT_IPHLPAPI_A_SRCS := $(wildcard libc/nt/iphlpapi/*.S)
LIBC_NT_IPHLPAPI_A_OBJS = $(LIBC_NT_IPHLPAPI_A_SRCS:%.S=o/$(MODE)/%.o)
LIBC_NT_IPHLPAPI_A_CHECKS = $(LIBC_IPHLPAPI_32_A).pkg
LIBC_NT_IPHLPAPI_A_DIRECTDEPS = LIBC_NT_KERNEL32
LIBC_NT_IPHLPAPI_A_DEPS :=				\
	$(call uniq,$(foreach x,$(LIBC_NT_IPHLPAPI_A_DIRECTDEPS),$($(x))))

$(LIBC_NT_IPHLPAPI_A):					\
		libc/nt/iphlpapi/			\
		$(LIBC_NT_IPHLPAPI_A).pkg		\
		$(LIBC_NT_IPHLPAPI_A_OBJS)

$(LIBC_NT_IPHLPAPI_A).pkg:				\
		$(LIBC_NT_IPHLPAPI_A_OBJS)		\
		$(foreach x,$(LIBC_NT_IPHLPAPI_A_DIRECTDEPS),$($(x)_A).pkg)

#───────────────────────────────────────────────────────────────────────────────

LIBC_NT_ARTIFACTS += LIBC_NT_IPHLPAPI_A
LIBC_NT_IPHLPAPI = $(LIBC_NT_IPHLPAPI_A_DEPS) $(LIBC_NT_IPHLPAPI_A)
LIBC_NT_IPHLPAPI_A = o/$(MODE)/libc/nt/iphlpapi.a
LIBC_NT_IPHLPAPI_A_SRCS := $(wildcard libc/nt/iphlpapi/*.S)
LIBC_NT_IPHLPAPI_A_OBJS = $(LIBC_NT_IPHLPAPI_A_SRCS:%.S=o/$(MODE)/%.o)
LIBC_NT_IPHLPAPI_A_CHECKS = $(LIBC_NT_IPHLPAPI_A).pkg
LIBC_NT_IPHLPAPI_A_DIRECTDEPS = LIBC_NT_KERNEL32
LIBC_NT_IPHLPAPI_A_DEPS :=				\
	$(call uniq,$(foreach x,$(LIBC_NT_IPHLPAPI_A_DIRECTDEPS),$($(x))))
$(LIBC_NT_IPHLPAPI_A):					\
		libc/nt/iphlpapi/			\
		$(LIBC_NT_IPHLPAPI_A).pkg		\
		$(LIBC_NT_IPHLPAPI_A_OBJS)
$(LIBC_NT_IPHLPAPI_A).pkg:				\
		$(LIBC_NT_IPHLPAPI_A_OBJS)		\
		$(foreach x,$(LIBC_NT_IPHLPAPI_A_DIRECTDEPS),$($(x)_A).pkg)

#───────────────────────────────────────────────────────────────────────────────

LIBC_NT_ARTIFACTS += LIBC_NT_POWRPROF_A
LIBC_NT_POWRPROF = $(LIBC_NT_POWRPROF_A_DEPS) $(LIBC_NT_POWRPROF_A)
LIBC_NT_POWRPROF_A = o/$(MODE)/libc/nt/powrprof.a
LIBC_NT_POWRPROF_A_SRCS := $(wildcard libc/nt/PowrProf/*.S)
LIBC_NT_POWRPROF_A_OBJS = $(LIBC_NT_POWRPROF_A_SRCS:%.S=o/$(MODE)/%.o)
LIBC_NT_POWRPROF_A_CHECKS = $(LIBC_NT_POWRPROF_A).pkg
LIBC_NT_POWRPROF_A_DIRECTDEPS = LIBC_NT_KERNEL32
LIBC_NT_POWRPROF_A_DEPS :=				\
	$(call uniq,$(foreach x,$(LIBC_NT_POWRPROF_A_DIRECTDEPS),$($(x))))
$(LIBC_NT_POWRPROF_A):					\
		libc/nt/PowrProf/			\
		$(LIBC_NT_POWRPROF_A).pkg		\
		$(LIBC_NT_POWRPROF_A_OBJS)
$(LIBC_NT_POWRPROF_A).pkg:				\
		$(LIBC_NT_POWRPROF_A_OBJS)		\
		$(foreach x,$(LIBC_NT_POWRPROF_A_DIRECTDEPS),$($(x)_A).pkg)

#───────────────────────────────────────────────────────────────────────────────

LIBC_NT_ARTIFACTS += LIBC_NT_PDH_A
LIBC_NT_PDH = $(LIBC_NT_PDH_A_DEPS) $(LIBC_NT_PDH_A)
LIBC_NT_PDH_A = o/$(MODE)/libc/nt/pdh.a
LIBC_NT_PDH_A_SRCS := $(wildcard libc/nt/pdh/*.S)
LIBC_NT_PDH_A_OBJS = $(LIBC_NT_PDH_A_SRCS:%.S=o/$(MODE)/%.o)
LIBC_NT_PDH_A_CHECKS = $(LIBC_NT_PDH_A).pkg
LIBC_NT_PDH_A_DIRECTDEPS = LIBC_NT_KERNEL32
LIBC_NT_PDH_A_DEPS :=					\
	$(call uniq,$(foreach x,$(LIBC_NT_PDH_A_DIRECTDEPS),$($(x))))
$(LIBC_NT_PDH_A):					\
		libc/nt/pdh/				\
		$(LIBC_NT_PDH_A).pkg			\
		$(LIBC_NT_PDH_A_OBJS)
$(LIBC_NT_PDH_A).pkg:					\
		$(LIBC_NT_PDH_A_OBJS)			\
		$(foreach x,$(LIBC_NT_PDH_A_DIRECTDEPS),$($(x)_A).pkg)

#───────────────────────────────────────────────────────────────────────────────

LIBC_NT_ARTIFACTS += LIBC_NT_PSAPI_A
LIBC_NT_PSAPI = $(LIBC_NT_PSAPI_A_DEPS) $(LIBC_NT_PSAPI_A)
LIBC_NT_PSAPI_A = o/$(MODE)/libc/nt/psapi.a
LIBC_NT_PSAPI_A_SRCS := $(wildcard libc/nt/psapi/*.S)
LIBC_NT_PSAPI_A_OBJS = $(LIBC_NT_PSAPI_A_SRCS:%.S=o/$(MODE)/%.o)
LIBC_NT_PSAPI_A_CHECKS = $(LIBC_NT_PSAPI_A).pkg
LIBC_NT_PSAPI_A_DIRECTDEPS = LIBC_NT_KERNEL32
LIBC_NT_PSAPI_A_DEPS :=					\
	$(call uniq,$(foreach x,$(LIBC_NT_PSAPI_A_DIRECTDEPS),$($(x))))
$(LIBC_NT_PSAPI_A):					\
		libc/nt/psapi/				\
		$(LIBC_NT_PSAPI_A).pkg			\
		$(LIBC_NT_PSAPI_A_OBJS)
$(LIBC_NT_PSAPI_A).pkg:					\
		$(LIBC_NT_PSAPI_A_OBJS)			\
		$(foreach x,$(LIBC_NT_PSAPI_A_DIRECTDEPS),$($(x)_A).pkg)

#───────────────────────────────────────────────────────────────────────────────

LIBC_NT_ARTIFACTS += LIBC_NT_BCRYPTPRIMITIVES_A
LIBC_NT_BCRYPTPRIMITIVES = $(LIBC_NT_BCRYPTPRIMITIVES_A_DEPS) $(LIBC_NT_BCRYPTPRIMITIVES_A)
LIBC_NT_BCRYPTPRIMITIVES_A = o/$(MODE)/libc/nt/BCryptPrimitives.a
LIBC_NT_BCRYPTPRIMITIVES_A_SRCS := $(wildcard libc/nt/BCryptPrimitives/*.S)
LIBC_NT_BCRYPTPRIMITIVES_A_OBJS = $(LIBC_NT_BCRYPTPRIMITIVES_A_SRCS:%.S=o/$(MODE)/%.o)
LIBC_NT_BCRYPTPRIMITIVES_A_CHECKS = $(LIBC_NT_BCRYPTPRIMITIVES_A).pkg
LIBC_NT_BCRYPTPRIMITIVES_A_DIRECTDEPS = LIBC_NT_KERNEL32
LIBC_NT_BCRYPTPRIMITIVES_A_DEPS := $(call uniq,$(foreach x,$(LIBC_NT_BCRYPTPRIMITIVES_A_DIRECTDEPS),$($(x))))
$(LIBC_NT_BCRYPTPRIMITIVES_A):				\
		libc/nt/BCryptPrimitives/		\
		$(LIBC_NT_BCRYPTPRIMITIVES_A).pkg	\
		$(LIBC_NT_BCRYPTPRIMITIVES_A_OBJS)
$(LIBC_NT_BCRYPTPRIMITIVES_A).pkg:			\
		$(LIBC_NT_BCRYPTPRIMITIVES_A_OBJS)	\
		$(foreach x,$(LIBC_NT_BCRYPTPRIMITIVES_A_DIRECTDEPS),$($(x)_A).pkg)

#───────────────────────────────────────────────────────────────────────────────

# let aarch64 compile these
o/$(MODE)/libc/nt/%.o: libc/nt/%.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) $<

#───────────────────────────────────────────────────────────────────────────────

.PHONY:		o/$(MODE)/libc/nt
o/$(MODE)/libc/nt:					\
		$(LIBC_NT_LIBS)				\
		$(LIBC_NT_CHECKS)
