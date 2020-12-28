#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += LIBC_NT

LIBC_NT_LIBS = $(foreach x,$(LIBC_NT_ARTIFACTS),$($(x)))
LIBC_NT_HDRS = $(foreach x,$(LIBC_NT_ARTIFACTS),$($(x)_HDRS))
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

LIBC_NT_A_HDRS =					\
	$(filter %.h,$(LIBC_NT_A_FILES))

LIBC_NT_A_CHECKS = $(patsubst %,o/$(MODE)/%.ok,$(filter %.h,$(LIBC_NT_A_HDRS)))

#───────────────────────────────────────────────────────────────────────────────

LIBC_NT_ARTIFACTS += LIBC_NT_KERNEL32_A
LIBC_NT_KERNEL32 = $(LIBC_NT_KERNEL32_A_DEPS) $(LIBC_NT_KERNEL32_A)
LIBC_NT_KERNEL32_A = o/$(MODE)/libc/nt/kernel32.a
LIBC_NT_KERNEL32_A_SRCS := $(wildcard libc/nt/kernel32/*.s)
LIBC_NT_KERNEL32_A_OBJS = $(LIBC_NT_KERNEL32_A_SRCS:%.s=o/$(MODE)/%.o)
LIBC_NT_KERNEL32_A_CHECKS = $(LIBC_NT_KERNEL32_A).pkg
LIBC_NT_KERNEL32_A_DIRECTDEPS = LIBC_STUBS
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
LIBC_NT_ADVAPI32_A_SRCS := $(wildcard libc/nt/advapi32/*.s)
LIBC_NT_ADVAPI32_A_OBJS = $(LIBC_NT_ADVAPI32_A_SRCS:%.s=o/$(MODE)/%.o)
LIBC_NT_ADVAPI32_A_CHECKS = $(LIBC_NT_ADVAPI32_A).pkg
LIBC_NT_ADVAPI32_A_DIRECTDEPS = LIBC_STUBS
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
LIBC_NT_COMDLG32_A_SRCS := $(wildcard libc/nt/comdlg32/*.s)
LIBC_NT_COMDLG32_A_OBJS = $(LIBC_NT_COMDLG32_A_SRCS:%.s=o/$(MODE)/%.o)
LIBC_NT_COMDLG32_A_CHECKS = $(LIBC_NT_COMDLG32_A).pkg
LIBC_NT_COMDLG32_A_DIRECTDEPS = LIBC_STUBS
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
LIBC_NT_GDI32_A_SRCS := $(wildcard libc/nt/gdi32/*.s)
LIBC_NT_GDI32_A_OBJS = $(LIBC_NT_GDI32_A_SRCS:%.s=o/$(MODE)/%.o)
LIBC_NT_GDI32_A_CHECKS = $(LIBC_NT_GDI32_A).pkg
LIBC_NT_GDI32_A_DIRECTDEPS = LIBC_STUBS
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

LIBC_NT_ARTIFACTS += LIBC_NT_KERNELBASE_A
LIBC_NT_KERNELBASE = $(LIBC_NT_KERNELBASE_A_DEPS) $(LIBC_NT_KERNELBASE_A)
LIBC_NT_KERNELBASE_A = o/$(MODE)/libc/nt/KernelBase.a
LIBC_NT_KERNELBASE_A_SRCS := $(wildcard libc/nt/KernelBase/*.s)
LIBC_NT_KERNELBASE_A_OBJS = $(LIBC_NT_KERNELBASE_A_SRCS:%.s=o/$(MODE)/%.o)
LIBC_NT_KERNELBASE_A_CHECKS = $(LIBC_NT_KERNELBASE_A).pkg
LIBC_NT_KERNELBASE_A_DIRECTDEPS = LIBC_STUBS
LIBC_NT_KERNELBASE_A_DEPS :=				\
	$(call uniq,$(foreach x,$(LIBC_NT_KERNELBASE_A_DIRECTDEPS),$($(x))))

$(LIBC_NT_KERNELBASE_A):				\
		libc/nt/KernelBase/			\
		$(LIBC_NT_KERNELBASE_A).pkg		\
		$(LIBC_NT_KERNELBASE_A_OBJS)

$(LIBC_NT_KERNELBASE_A).pkg:				\
		$(LIBC_NT_KERNELBASE_A_OBJS)		\
		$(foreach x,$(LIBC_NT_KERNELBASE_A_DIRECTDEPS),$($(x)_A).pkg)

#───────────────────────────────────────────────────────────────────────────────

LIBC_NT_ARTIFACTS += LIBC_NT_NTDLL_A
LIBC_NT_NTDLL = $(LIBC_NT_NTDLL_A_DEPS) $(LIBC_NT_NTDLL_A)
LIBC_NT_NTDLL_A = o/$(MODE)/libc/nt/ntdll.a
LIBC_NT_NTDLL_A_SRCS_A := $(wildcard libc/nt/ntdll/*.s)
LIBC_NT_NTDLL_A_SRCS_S = libc/nt/ntdllimport.S
LIBC_NT_NTDLL_A_SRCS = $(LIBC_NT_NTDLL_A_SRCS_A) $(LIBC_NT_NTDLL_A_SRCS_S)
LIBC_NT_NTDLL_A_OBJS =					\
	$(LIBC_NT_NTDLL_A_SRCS_A:%.s=o/$(MODE)/%.o)	\
	$(LIBC_NT_NTDLL_A_SRCS_S:%.S=o/$(MODE)/%.o)
LIBC_NT_NTDLL_A_CHECKS = $(LIBC_NT_NTDLL_A).pkg
LIBC_NT_NTDLL_A_DIRECTDEPS = LIBC_STUBS  LIBC_NT_KERNEL32
LIBC_NT_NTDLL_A_DEPS :=					\
	$(call uniq,$(foreach x,$(LIBC_NT_NTDLL_A_DIRECTDEPS),$($(x))))

$(LIBC_NT_NTDLL_A):					\
		libc/nt/ntdll/				\
		$(LIBC_NT_NTDLL_A).pkg			\
		$(LIBC_NT_NTDLL_A_OBJS)
	@$(file >$@.cmd) $(file >>$@.cmd,$(ARCHIVE) $@ $^ >$(LIBC_NT_NTDLL_A).cmd)
	@$(ARCHIVE) $@ $^

$(LIBC_NT_NTDLL_A).pkg:					\
		$(LIBC_NT_NTDLL_A_OBJS)			\
		$(foreach x,$(LIBC_NT_NTDLL_A_DIRECTDEPS),$($(x)_A).pkg)

$(LIBC_NT_NTDLL_A_OBJS):				\
		o/libc/nt/ntdllimport.inc

o/libc/nt/ntdllimport.inc:				\
		ape/relocations.h			\
		libc/nt/ntdllimport.h			\
		libc/macros.h				\
		libc/macros.internal.inc		\
		libc/macros-cpp.internal.inc

#───────────────────────────────────────────────────────────────────────────────

LIBC_NT_ARTIFACTS += LIBC_NT_NETAPI32_A
LIBC_NT_NETAPI32 = $(LIBC_NT_NETAPI32_A_DEPS) $(LIBC_NT_NETAPI32_A)
LIBC_NT_NETAPI32_A = o/$(MODE)/libc/nt/netapi32.a
LIBC_NT_NETAPI32_A_SRCS := $(wildcard libc/nt/netapi32/*.s)
LIBC_NT_NETAPI32_A_OBJS = $(LIBC_NT_NETAPI32_A_SRCS:%.s=o/$(MODE)/%.o)
LIBC_NT_NETAPI32_A_CHECKS = $(LIBC_NT_NETAPI32_A).pkg
LIBC_NT_NETAPI32_A_DIRECTDEPS = LIBC_STUBS
LIBC_NT_NETAPI32_A_DEPS :=				\
	$(call uniq,$(foreach x,$(LIBC_NT_NETAPI32_A_DIRECTDEPS),$($(x))))

$(LIBC_NT_NETAPI32_A):					\
		libc/nt/netapi32/			\
		$(LIBC_NT_NETAPI32_A).pkg		\
		$(LIBC_NT_NETAPI32_A_OBJS)

$(LIBC_NT_NETAPI32_A).pkg:				\
		$(LIBC_NT_NETAPI32_A_OBJS)		\
		$(foreach x,$(LIBC_NT_NETAPI32_A_DIRECTDEPS),$($(x)_A).pkg)

#───────────────────────────────────────────────────────────────────────────────

LIBC_NT_ARTIFACTS += LIBC_NT_URL_A
LIBC_NT_URL = $(LIBC_NT_URL_A_DEPS) $(LIBC_NT_URL_A)
LIBC_NT_URL_A = o/$(MODE)/libc/nt/url.a
LIBC_NT_URL_A_SRCS := $(wildcard libc/nt/url/*.s)
LIBC_NT_URL_A_OBJS = $(LIBC_NT_URL_A_SRCS:%.s=o/$(MODE)/%.o)
LIBC_NT_URL_A_CHECKS = $(LIBC_NT_URL_A).pkg
LIBC_NT_URL_A_DIRECTDEPS = LIBC_STUBS
LIBC_NT_URL_A_DEPS :=					\
	$(call uniq,$(foreach x,$(LIBC_NT_URL_A_DIRECTDEPS),$($(x))))

$(LIBC_NT_URL_A):					\
		libc/nt/url/				\
		$(LIBC_NT_URL_A).pkg			\
		$(LIBC_NT_URL_A_OBJS)

$(LIBC_NT_URL_A).pkg:					\
		$(LIBC_NT_URL_A_OBJS)			\
		$(foreach x,$(LIBC_NT_URL_A_DIRECTDEPS),$($(x)_A).pkg)

#───────────────────────────────────────────────────────────────────────────────

LIBC_NT_ARTIFACTS += LIBC_NT_USER32_A
LIBC_NT_USER32 = $(LIBC_NT_USER32_A_DEPS) $(LIBC_NT_USER32_A)
LIBC_NT_USER32_A = o/$(MODE)/libc/nt/user32.a
LIBC_NT_USER32_A_SRCS := $(wildcard libc/nt/user32/*.s)
LIBC_NT_USER32_A_OBJS = $(LIBC_NT_USER32_A_SRCS:%.s=o/$(MODE)/%.o)
LIBC_NT_USER32_A_CHECKS = $(LIBC_NT_USER32_A).pkg
LIBC_NT_USER32_A_DIRECTDEPS = LIBC_STUBS
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
LIBC_NT_WS2_32_A_SRCS := $(wildcard libc/nt/ws2_32/*.s)
LIBC_NT_WS2_32_A_OBJS = $(LIBC_NT_WS2_32_A_SRCS:%.s=o/$(MODE)/%.o)
LIBC_NT_WS2_32_A_CHECKS = $(LIBC_NT_WS2_32_A).pkg
LIBC_NT_WS2_32_A_DIRECTDEPS = LIBC_STUBS
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

LIBC_NT_ARTIFACTS += LIBC_NT_MSWSOCK_A
LIBC_NT_MSWSOCK = $(LIBC_NT_MSWSOCK_A_DEPS) $(LIBC_NT_MSWSOCK_A)
LIBC_NT_MSWSOCK_A = o/$(MODE)/libc/nt/MsWSock.a
LIBC_NT_MSWSOCK_A_SRCS := $(wildcard libc/nt/MsWSock/*.s)
LIBC_NT_MSWSOCK_A_OBJS = $(LIBC_NT_MSWSOCK_A_SRCS:%.s=o/$(MODE)/%.o)
LIBC_NT_MSWSOCK_A_CHECKS = $(LIBC_NT_MSWSOCK_A).pkg
LIBC_NT_MSWSOCK_A_DIRECTDEPS = LIBC_STUBS
LIBC_NT_MSWSOCK_A_DEPS :=				\
	$(call uniq,$(foreach x,$(LIBC_NT_MSWSOCK_A_DIRECTDEPS),$($(x))))

$(LIBC_NT_MSWSOCK_A):					\
		libc/nt/MsWSock/			\
		$(LIBC_NT_MSWSOCK_A).pkg		\
		$(LIBC_NT_MSWSOCK_A_OBJS)

$(LIBC_NT_MSWSOCK_A).pkg:				\
		$(LIBC_NT_MSWSOCK_A_OBJS)		\
		$(foreach x,$(LIBC_NT_MSWSOCK_A_DIRECTDEPS),$($(x)_A).pkg)

#───────────────────────────────────────────────────────────────────────────────

LIBC_NT_ARTIFACTS += LIBC_NT_SHELL32_A
LIBC_NT_SHELL32 = $(LIBC_NT_SHELL32_A_DEPS) $(LIBC_NT_SHELL32_A)
LIBC_NT_SHELL32_A = o/$(MODE)/libc/nt/shell32.a
LIBC_NT_SHELL32_A_SRCS := $(wildcard libc/nt/shell32/*.s)
LIBC_NT_SHELL32_A_OBJS = $(LIBC_NT_SHELL32_A_SRCS:%.s=o/$(MODE)/%.o)
LIBC_NT_SHELL32_A_CHECKS = $(LIBC_NT_SHELL32_A).pkg
LIBC_NT_SHELL32_A_DIRECTDEPS = LIBC_STUBS
LIBC_NT_SHELL32_A_DEPS :=				\
	$(call uniq,$(foreach x,$(LIBC_NT_SHELL32_A_DIRECTDEPS),$($(x))))
$(LIBC_NT_SHELL32_A):					\
		libc/nt/shell32/			\
		$(LIBC_NT_SHELL32_A).pkg		\
		$(LIBC_NT_SHELL32_A_OBJS)
$(LIBC_NT_SHELL32_A).pkg:				\
		$(LIBC_NT_SHELL32_A_OBJS)		\
		$(foreach x,$(LIBC_NT_SHELL32_A_DIRECTDEPS),$($(x)_A).pkg)

#───────────────────────────────────────────────────────────────────────────────

$(LIBC_NT_OBJS): o/libc/nt/codegen.inc

.PHONY:		o/$(MODE)/libc/nt
o/$(MODE)/libc/nt:					\
		$(LIBC_NT_LIBS)				\
		$(LIBC_NT_CHECKS)
