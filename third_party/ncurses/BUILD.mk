#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_NCURSES

THIRD_PARTY_NCURSES_ARTIFACTS += THIRD_PARTY_NCURSES_A
THIRD_PARTY_NCURSES = $(THIRD_PARTY_NCURSES_A_DEPS) $(THIRD_PARTY_NCURSES_A)
THIRD_PARTY_NCURSES_A = o/$(MODE)/third_party/ncurses/ncurses.a
THIRD_PARTY_NCURSES_A_FILES := $(wildcard third_party/ncurses/*)
THIRD_PARTY_NCURSES_A_HDRS = $(filter %.h,$(THIRD_PARTY_NCURSES_A_FILES))
THIRD_PARTY_NCURSES_A_INCS = $(filter %.inc,$(THIRD_PARTY_NCURSES_A_FILES))
THIRD_PARTY_NCURSES_A_SRCS = $(filter %.c,$(THIRD_PARTY_NCURSES_A_FILES))
THIRD_PARTY_NCURSES_A_TERM := $(wildcard usr/share/terminfo/*/*)
THIRD_PARTY_NCURSES_A_SRCOBJS = $(THIRD_PARTY_NCURSES_A_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_NCURSES_A_OBJS =				\
	$(THIRD_PARTY_NCURSES_A_SRCOBJS)		\
	$(THIRD_PARTY_NCURSES_A_TERM:%=o/$(MODE)/%.zip.o)

THIRD_PARTY_NCURSES_A_CHECKS =				\
	$(THIRD_PARTY_NCURSES_A).pkg

THIRD_PARTY_NCURSES_A_DIRECTDEPS =			\
	LIBC_CALLS					\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_PROC					\
	LIBC_RUNTIME					\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_SYSV					\
	THIRD_PARTY_MUSL

THIRD_PARTY_NCURSES_A_DEPS :=				\
	$(call uniq,$(foreach x,$(THIRD_PARTY_NCURSES_A_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_NCURSES_A):				\
		third_party/ncurses/			\
		$(THIRD_PARTY_NCURSES_A).pkg		\
		$(THIRD_PARTY_NCURSES_A_OBJS)

$(THIRD_PARTY_NCURSES_A).pkg:				\
		$(THIRD_PARTY_NCURSES_A_OBJS)		\
		$(foreach x,$(THIRD_PARTY_NCURSES_A_DIRECTDEPS),$($(x)_A).pkg)

$(THIRD_PARTY_NCURSES_A_SRCOBJS): private		\
		CFLAGS +=				\
			-fportcosmo			\
			--param max-inline-insns-single=1200

$(THIRD_PARTY_NCURSES_A_SRCOBJS): private		\
		CPPFLAGS +=				\
			-DNDEBUG			\
			--param max-inline-insns-single=1200

o/$(MODE)/third_party/ncurses/define_key.o		\
o/$(MODE)/third_party/ncurses/key_defined.o		\
o/$(MODE)/third_party/ncurses/keybound.o		\
o/$(MODE)/third_party/ncurses/keyok.o			\
o/$(MODE)/third_party/ncurses/tries.o			\
o/$(MODE)/third_party/ncurses/version.o			\
o/$(MODE)/third_party/ncurses/access.o			\
o/$(MODE)/third_party/ncurses/add_tries.o		\
o/$(MODE)/third_party/ncurses/alloc_ttype.o		\
o/$(MODE)/third_party/ncurses/comp_error.o		\
o/$(MODE)/third_party/ncurses/comp_hash.o		\
o/$(MODE)/third_party/ncurses/db_iterator.o		\
o/$(MODE)/third_party/ncurses/doalloc.o			\
o/$(MODE)/third_party/ncurses/entries.o			\
o/$(MODE)/third_party/ncurses/free_ttype.o		\
o/$(MODE)/third_party/ncurses/getenv_num.o		\
o/$(MODE)/third_party/ncurses/hashed_db.o		\
o/$(MODE)/third_party/ncurses/home_terminfo.o		\
o/$(MODE)/third_party/ncurses/init_keytry.o		\
o/$(MODE)/third_party/ncurses/lib_acs.o			\
o/$(MODE)/third_party/ncurses/lib_baudrate.o		\
o/$(MODE)/third_party/ncurses/lib_cur_term.o		\
o/$(MODE)/third_party/ncurses/lib_data.o		\
o/$(MODE)/third_party/ncurses/lib_has_cap.o		\
o/$(MODE)/third_party/ncurses/lib_kernel.o		\
o/$(MODE)/third_party/ncurses/lib_longname.o		\
o/$(MODE)/third_party/ncurses/lib_napms.o		\
o/$(MODE)/third_party/ncurses/lib_options.o		\
o/$(MODE)/third_party/ncurses/lib_raw.o			\
o/$(MODE)/third_party/ncurses/lib_setup.o		\
o/$(MODE)/third_party/ncurses/lib_termcap.o		\
o/$(MODE)/third_party/ncurses/lib_termname.o		\
o/$(MODE)/third_party/ncurses/lib_tgoto.o		\
o/$(MODE)/third_party/ncurses/lib_ti.o			\
o/$(MODE)/third_party/ncurses/lib_tparm.o		\
o/$(MODE)/third_party/ncurses/lib_tputs.o		\
o/$(MODE)/third_party/ncurses/lib_ttyflags.o		\
o/$(MODE)/third_party/ncurses/name_match.o		\
o/$(MODE)/third_party/ncurses/obsolete.o		\
o/$(MODE)/third_party/ncurses/read_entry.o		\
o/$(MODE)/third_party/ncurses/read_termcap.o		\
o/$(MODE)/third_party/ncurses/strings.o			\
o/$(MODE)/third_party/ncurses/trim_sgr0.o		\
o/$(MODE)/third_party/ncurses/lib_trace.o		\
o/$(MODE)/third_party/ncurses/lib_traceatr.o		\
o/$(MODE)/third_party/ncurses/lib_tracebits.o		\
o/$(MODE)/third_party/ncurses/lib_tracechr.o		\
o/$(MODE)/third_party/ncurses/trace_buf.o		\
o/$(MODE)/third_party/ncurses/trace_tries.o		\
o/$(MODE)/third_party/ncurses/visbuf.o			\
o/$(MODE)/third_party/ncurses/lib_twait.o		\
o/$(MODE)/third_party/ncurses/codes.o			\
o/$(MODE)/third_party/ncurses/comp_captab.o		\
o/$(MODE)/third_party/ncurses/comp_userdefs.o		\
o/$(MODE)/third_party/ncurses/fallback.o		\
o/$(MODE)/third_party/ncurses/lib_keyname.o		\
o/$(MODE)/third_party/ncurses/names.o			\
o/$(MODE)/third_party/ncurses/unctrl.o: private		\
		CPPFLAGS +=				\
			-DUSE_TERMLIB

THIRD_PARTY_NCURSES_BINS = $(THIRD_PARTY_NCURSES_COMS) $(THIRD_PARTY_NCURSES_COMS:%=%.dbg)
THIRD_PARTY_NCURSES_LIBS = $(foreach x,$(THIRD_PARTY_NCURSES_ARTIFACTS),$($(x)))
THIRD_PARTY_NCURSES_SRCS = $(foreach x,$(THIRD_PARTY_NCURSES_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_NCURSES_HDRS = $(foreach x,$(THIRD_PARTY_NCURSES_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_NCURSES_INCS = $(foreach x,$(THIRD_PARTY_NCURSES_ARTIFACTS),$($(x)_INCS))
THIRD_PARTY_NCURSES_CHECKS = $(foreach x,$(THIRD_PARTY_NCURSES_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_NCURSES_OBJS = $(foreach x,$(THIRD_PARTY_NCURSES_ARTIFACTS),$($(x)_OBJS))
$(THIRD_PARTY_NCURSES_A_SRCOBJS): $(BUILD_FILES) third_party/ncurses/BUILD.mk

.PHONY: o/$(MODE)/third_party/ncurses
o/$(MODE)/third_party/ncurses:				\
		$(THIRD_PARTY_NCURSES_A)		\
		$(THIRD_PARTY_NCURSES_BINS)		\
		$(THIRD_PARTY_NCURSES_CHECKS)
