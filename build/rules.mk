#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘
#
# SYNOPSIS
#
#   Cosmopolitan Core Build Rules
#
# DESCRIPTION
#
#   This file shows GNU Make how to convert between file types, based on
#   their extensions. We use everyday, ordinary, boring, and ubiquitous
#   system commands for everything, e.g. as, cc, ld, etc. with plain and
#   simple shell-script wrappers, e.g. build/assemble and build/compile.
#   Those veneers abstract away most of the boring trivialities, to keep
#   our makefiles pristine and readable.

MAKEFLAGS += --no-builtin-rules

o/%.o: %.s                         ; @$(COMPILE) -AOBJECTIFY.s $(OBJECTIFY.s) $(OUTPUT_OPTION) $<
o/%.o: o/%.s                       ; @$(COMPILE) -AOBJECTIFY.s $(OBJECTIFY.s) $(OUTPUT_OPTION) $<
o/%.s: %.S                         ; @$(COMPILE) -APREPROCESS $(PREPROCESS) $(OUTPUT_OPTION) $<
o/%.s: o/%.S                       ; @$(COMPILE) -APREPROCESS $(PREPROCESS) $(OUTPUT_OPTION) $<
o/%.o: %.S                         ; @$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) $<
o/%.o: o/%.S                       ; @$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) $<
o/%.lds: %.lds                     ; @$(COMPILE) -APREPROCESS $(PREPROCESS.lds) $(OUTPUT_OPTION) $<
o/%.inc: %.h                       ; @$(COMPILE) -APREPROCESS $(PREPROCESS) $(OUTPUT_OPTION) -D__ASSEMBLER__ -P $<
o/%.greg.o: %.greg.c               ; @$(COMPILE) -AOBJECTIFY.greg $(OBJECTIFY.greg.c) $(OUTPUT_OPTION) $<

o/$(MODE)/%: o/$(MODE)/%.dbg       ; @$(COMPILE) -AOBJCOPY -T$@ $(OBJCOPY) -S -O binary $< $@
o/$(MODE)/%.o: %.s                 ; @$(COMPILE) -AOBJECTIFY.s $(OBJECTIFY.s) $(OUTPUT_OPTION) $<
o/$(MODE)/%.o: o/$(MODE)/%.s       ; @$(COMPILE) -AOBJECTIFY.s $(OBJECTIFY.s) $(OUTPUT_OPTION) $<
o/$(MODE)/%.o: %.f                 ; @$(COMPILE) -AOBJECTIFY.f $(OBJECTIFY.f) $(OUTPUT_OPTION) $<
o/$(MODE)/%.o: %.F                 ; @$(COMPILE) -AOBJECTIFY.F $(OBJECTIFY.F) $(OUTPUT_OPTION) $<
o/$(MODE)/%.h: %.c                 ; @$(COMPILE) -AAMALGAMATE $(PREPROCESS) $(OUTPUT_OPTION) -fdirectives-only -P $<
o/$(MODE)/%.h: o/$(MODE)/%.c       ; @$(COMPILE) -AAMALGAMATE $(PREPROCESS) $(OUTPUT_OPTION) -fdirectives-only -P $<
o/$(MODE)/%.o: %.S                 ; @$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) $<
o/$(MODE)/%.o: o/$(MODE)/%.S       ; @$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) $<
o/$(MODE)/%.o: %.cc                ; @$(COMPILE) -AOBJECTIFY.cxx $(OBJECTIFY.cxx) $(OUTPUT_OPTION) $<
o/$(MODE)/%.lds: %.lds             ; @$(COMPILE) -APREPROCESS $(PREPROCESS.lds) $(OUTPUT_OPTION) $<
o/$(MODE)/%.o: %.greg.c            ; @$(COMPILE) -AOBJECTIFY.greg $(OBJECTIFY.greg.c) $(OUTPUT_OPTION) $<
o/$(MODE)/%.greg.o: %.greg.c       ; @$(COMPILE) -AOBJECTIFY.greg $(OBJECTIFY.greg.c) $(OUTPUT_OPTION) $<
o/$(MODE)/%.initabi.o: %.initabi.c ; @$(COMPILE) -AOBJECTIFY.init $(OBJECTIFY.initabi.c) $(OUTPUT_OPTION) $<
o/$(MODE)/%.ncabi.o: %.ncabi.c     ; @$(COMPILE) -AOBJECTIFY.nc $(OBJECTIFY.ncabi.c) $(OUTPUT_OPTION) $<
o/$(MODE)/%.real.o: %.c            ; @$(COMPILE) -AOBJECTIFY.real $(OBJECTIFY.real.c) $(OUTPUT_OPTION) $<

o/%.o: %.cc
	@$(COMPILE) -AOBJECTIFY.cxx $(OBJECTIFY.cxx) $(OUTPUT_OPTION) $<
	@$(COMPILE) -AFIXUPOBJ -wT$@ $(FIXUPOBJ) $@

o/%.o: o/%.cc
	@$(COMPILE) -AOBJECTIFY.cxx $(OBJECTIFY.cxx) $(OUTPUT_OPTION) $<
	@$(COMPILE) -AFIXUPOBJ -wT$@ $(FIXUPOBJ) $@

o/$(MODE)/%.o: %.c
	@$(COMPILE) -AOBJECTIFY.c $(OBJECTIFY.c) $(OUTPUT_OPTION) $<
	@$(COMPILE) -AFIXUPOBJ -wT$@ $(FIXUPOBJ) $@

o/$(MODE)/%.o: o/$(MODE)/%.c
	@$(COMPILE) -AOBJECTIFY.c $(OBJECTIFY.c) $(OUTPUT_OPTION) $<
	@$(COMPILE) -AFIXUPOBJ -wT$@ $(FIXUPOBJ) $@

o/$(MODE)/%.o: o/$(MODE)/%.cc
	@$(COMPILE) -AOBJECTIFY.cxx $(OBJECTIFY.cxx) $(OUTPUT_OPTION) $<
	@$(COMPILE) -AFIXUPOBJ -wT$@ $(FIXUPOBJ) $@

o/%.a:
	$(file >$(TMPDIR)/$(subst /,_,$@),$^)
	@$(COMPILE) -AARCHIVE -wT$@ $(AR) $(ARFLAGS) $@ @$(TMPDIR)/$(subst /,_,$@)

o/%.pkg:
	$(file >$(TMPSAFE).args,$(filter %.o,$^))
	@$(COMPILE) -APACKAGE -wT$@ $(PKG) $(OUTPUT_OPTION) $(addprefix -d,$(filter %.pkg,$^)) @$(TMPSAFE).args

o/$(MODE)/%.pkg:
	$(file >$(TMPSAFE).args,$(filter %.o,$^))
	@$(COMPILE) -APACKAGE -wT$@ $(PKG) $(OUTPUT_OPTION) $(addprefix -d,$(filter %.pkg,$^)) @$(TMPSAFE).args

o/$(MODE)/%.o: %.py o/$(MODE)/third_party/python/pyobj.com
	@$(COMPILE) -wAPYOBJ o/$(MODE)/third_party/python/pyobj.com $(PYFLAGS) -o $@ $<

o/$(MODE)/%.pyc: %.py o/$(MODE)/third_party/python/pycomp.com
	@$(COMPILE) -wAPYCOMP o/$(MODE)/third_party/python/pycomp.com $(PYCFLAGS) -o $@ $<

o/$(MODE)/%.lua: %.lua o/$(MODE)/third_party/lua/luac.com
	@$(COMPILE) -wALUAC o/$(MODE)/third_party/lua/luac.com -s -o $@ $<

o/$(MODE)/%.lua.runs: %.lua o/$(MODE)/tool/net/redbean.com
	@$(COMPILE) -wALUA -tT$@ o/$(MODE)/tool/net/redbean.com $(LUAFLAGS) -i $<

o/$(MODE)/%: o/$(MODE)/%.com o/$(MODE)/tool/build/cp.com o/$(MODE)/tool/build/assimilate.com
	@$(COMPILE) -wACP -T$@ o/$(MODE)/tool/build/cp.com $< $@
	@$(COMPILE) -wAASSIMILATE -T$@ o/$(MODE)/tool/build/assimilate.com $@

################################################################################
# LOCAL UNIT TESTS
#
# We always run unit tests as part of the normal `make` invocation. you
# may override the $(TESTARGS) variable to do things such as enable the
# benchmarking feature. For example:
#
#     TESTARGS = -b
#
# May be specified in your ~/.cosmo.mk file. You can also use this to
# enable things like function tracing. For example:
#
#     TESTARGS = ----ftrace
#     .PLEDGE += prot_exec
#
# You could then run a command like:
#
#     make -j8 o//test/libc/calls/openbsd_test.com.runs
#
# You need PROT_EXEC permission since ftrace morphs the binary. It's
# also worth mentioning that the pledge.com command can simulate what
# Landlock Make does:
#
#     o//tool/build/pledge.com \
#       -v. -p 'stdio rpath wpath cpath tty prot_exec' \
#       o//test/libc/calls/openbsd_test.com \
#       ----ftrace
#
# This is useful in the event a test binary should run by itself, but
# fails to run beneath Landlock Make. It's also useful sometimes to
# override the verbosity when running tests:
#
#     make V=5 TESTARGS=-b o//test/libc/calls/openbsd_test.com.runs
#
# This way, if for some reason a test should fail but calls exit(0),
# then the stdout/stderr output, which would normally be suppressed,
# will actually be displayed.

o/$(MODE)/%.runs: o/$(MODE)/%
	@$(COMPILE) -ACHECK -wtT$@ $< $(TESTARGS)

################################################################################
# ELF ZIP FILES
#
# zipobj.com lets us do fast incremental linking of compressed data.
# it's nice because if we link a hundred binaries that use the time zone
# database, then that database only needs to be DEFLATE'd once.

o/%.zip.o: o/%
	@$(COMPILE) -wAZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/%.zip.o: %
	@$(COMPILE) -wAZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/%.zip.o: %
	@$(COMPILE) -wAZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

# an issue with sandboxing arises when creating directory entries in the
# zip file. we need the trailing slash (e.g. o//foo/.zip.o) but Landlock
# Make avoids sandboxing directory names that have a trailing slash (so
# they can be used to watch for deleted files, without creating overly
# broad unveiling). such rules need to be written more explicitly.
o/$(MODE)%/.zip.o: %
	@$(COMPILE) -wAZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

################################################################################
# STRICT HEADER CHECKING
#
# these rules are unsandboxed since they're already a sandboxing test,
# and it would be too costly in terms of make latency to have every
# header file depend on $(HDRS) and $(INCS).

o/%.h.ok: private .UNSANDBOXED = 1
o/%.h.ok: %.h
	@$(COMPILE) -ACHECK.h $(COMPILE.c) -xc -g0 -o $@ $<

o/$(MODE)/%.h.ok: private .UNSANDBOXED = 1
o/$(MODE)/%.h.ok: %.h
	@$(COMPILE) -ACHECK.h $(COMPILE.c) -xc -g0 -o $@ $<

o/$(MODE)/%.hh.ok: private .UNSANDBOXED = 1
o/$(MODE)/%.hh.ok: %.hh
	@$(COMPILE) -ACHECK.h $(COMPILE.cxx) -xc++ -g0 -o $@ $<

o/%.okk: .UNSANDBOXED = 1
o/%.okk: %
	@$(COMPILE) -ACHECK.h $(COMPILE.cxx) -xc++ -g0 -o $@ $<

o/$(MODE)/%.okk: private .UNSANDBOXED = 1
o/$(MODE)/%.okk: %
	@$(COMPILE) -ACHECK.h $(COMPILE.cxx) -xc++ -g0 -o $@ $<

################################################################################
# EXECUTABLE HELPERS

MAKE_OBJCOPY =					\
	$(COMPILE) -AOBJCOPY -T$@		\
	$(OBJCOPY) -S -O binary $< $@

MAKE_SYMTAB_CREATE =				\
	$(COMPILE) -wASYMTAB			\
	o/$(MODE)/tool/build/symtab.com		\
	-o $(TMPSAFE)/.symtab			\
	$<

MAKE_SYMTAB_ZIP =				\
	$(COMPILE) -AZIP -T$@			\
	o/$(MODE)/third_party/zip/zip.com	\
	-b$(TMPDIR)				\
	-9qj					\
	$@					\
	$(TMPSAFE)/.symtab

################################################################################
# EMACS ASSEMBLY GENERATION

o/$(MODE)/%-gcc.asm: private .UNSANDBOXED = 1
o/$(MODE)/%-gcc.asm: %.c
	@$(COMPILE) -AOBJECTIFY.c $(OBJECTIFY.c) -S -g0 $(OUTPUT_OPTION) $<

o/$(MODE)/%-gcc.asm: private .UNSANDBOXED = 1
o/$(MODE)/%-gcc.asm: %.cc
	@$(COMPILE) -AOBJECTIFY.c $(OBJECTIFY.cxx) -S -g0 $(OUTPUT_OPTION) $<

o/$(MODE)/%-clang.asm: private .UNSANDBOXED = 1
o/$(MODE)/%-clang.asm: %.c
	@$(COMPILE) -AOBJECTIFY.c $(OBJECTIFY.c) -S -g0 $(OUTPUT_OPTION) $<

o/$(MODE)/%-clang.asm: CC = $(CLANG)
o/$(MODE)/%-clang.asm: private .UNSANDBOXED = 1
o/$(MODE)/%-clang.asm: %.cc
	@$(COMPILE) -AOBJECTIFY.c $(OBJECTIFY.cxx) -S -g0 $(OUTPUT_OPTION) $<
