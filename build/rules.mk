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

o/%.a:; @$(ARCHIVE) $@ $^
o/%.o: %.s; @TARGET=$@ $(COMPILE) $(OBJECTIFY.s) $(OUTPUT_OPTION) $<
o/%.o: o/%.s; @TARGET=$@ $(COMPILE) $(OBJECTIFY.s) $(OUTPUT_OPTION) $<
o/%.s: %.S; @ACTION=PREPROCESS $(COMPILE) $(PREPROCESS) $(OUTPUT_OPTION) $<
o/%.s: o/%.S; @ACTION=PREPROCESS $(COMPILE) $(PREPROCESS) $(OUTPUT_OPTION) $<
o/%.i: %.S; @ACTION=PREPROCESS $(COMPILE) $(PREPROCESS) $(OUTPUT_OPTION) $<
o/%.o: %.S; @ACTION=OBJECTIFY.S $(COMPILE) $(OBJECTIFY.S) $(OUTPUT_OPTION) $<
o/%.o: o/%.S; @ACTION=OBJECTIFY.S $(COMPILE) $(OBJECTIFY.S) $(OUTPUT_OPTION) $<
o/%.s: %.i; @ACTION=COMPILE.i $(COMPILE) $(COMPILE.i) $(OUTPUT_OPTION) $<
o/%.s: o/%.i; @ACTION=COMPILE.i $(COMPILE) $(COMPILE.i) $(OUTPUT_OPTION) $<
o/%.o: %.cc; @ACTION=OBJECTIFY.cxx $(COMPILE) $(OBJECTIFY.cxx) $(OUTPUT_OPTION) $<
o/%.o: o/%.cc; @ACTION=OBJECTIFY.cxx $(COMPILE) $(OBJECTIFY.cxx) $(OUTPUT_OPTION) $<
o/%.lds: %.lds; @ACTION=PREPROCESS $(COMPILE) $(PREPROCESS.lds) $(OUTPUT_OPTION) $<
o/%.inc: %.h; @ACTION=PREPROCESS $(COMPILE) $(PREPROCESS) $(OUTPUT_OPTION) -D__ASSEMBLER__ -P $<
o/%.pkg:; @build/package $(OUTPUT_OPTION) $(addprefix -d,$(filter %.pkg,$^)) $(filter %.o,$^)
o/%.h.ok: %.h; @ACTION=CHECK.h $(COMPILE) $(COMPILE.c) -x c -g0 -o $@ $<
o/%.h.okk: %.h; @ACTION=CHECK.h $(COMPILE) $(COMPILE.cxx) -x c++ -g0 -o $@ $<
o/%.greg.o: %.greg.c; @ACTION=OBJECTIFY.greg $(COMPILE) $(OBJECTIFY.greg.c) $(OUTPUT_OPTION) $<
o/%.zip.o: o/%; @build/zipobj $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/%.a:; @$(ARCHIVE) $@ $^
o/$(MODE)/%: o/$(MODE)/%.dbg; @ACTION=OBJCOPY TARGET=$@ $(COMPILE) $(OBJCOPY) -S -O binary $< $@
o/$(MODE)/%.o: %.s; @TARGET=$@ $(COMPILE) $(OBJECTIFY.s) $(OUTPUT_OPTION) $<
o/$(MODE)/%.o: o/$(MODE)/%.s; @TARGET=$@ $(COMPILE) $(OBJECTIFY.s) $(OUTPUT_OPTION) $<
o/$(MODE)/%.s: %.S; @ACTION=PREPROCESS $(COMPILE) $(PREPROCESS) $(OUTPUT_OPTION) $<
o/$(MODE)/%.s: o/$(MODE)/%.S; @ACTION=PREPROCESS $(COMPILE) $(PREPROCESS) $(OUTPUT_OPTION) $<
o/$(MODE)/%.o: %.c; @ACTION=OBJECTIFY.c $(COMPILE) $(OBJECTIFY.c) $(OUTPUT_OPTION) $<
o/$(MODE)/%.o: %.f; @ACTION=OBJECTIFY.f $(COMPILE) $(OBJECTIFY.f) $(OUTPUT_OPTION) $<
o/$(MODE)/%.o: %.F; @ACTION=OBJECTIFY.F $(COMPILE) $(OBJECTIFY.F) $(OUTPUT_OPTION) $<
o/$(MODE)/%.o: o/$(MODE)/%.c; @ACTION=OBJECTIFY.c $(COMPILE) $(OBJECTIFY.c) $(OUTPUT_OPTION) $<
o/$(MODE)/%.ss: %.c; @ACTION=COMPILE.c $(COMPILE) $(COMPILE.c) $(OUTPUT_OPTION) $<
o/$(MODE)/%.ss: o/$(MODE)/%.c; @ACTION=OBJECTIFY.s $(COMPILE) $(COMPILE.c) $(OUTPUT_OPTION) $<
o/$(MODE)/%.i: %.S; @ACTION=PREPROCESS $(COMPILE) $(PREPROCESS) $(OUTPUT_OPTION) $<
o/$(MODE)/%.i: %.c; @ACTION=PREPROCESS $(COMPILE) $(PREPROCESS) $(OUTPUT_OPTION) $<
o/$(MODE)/%.i: %.cc; @ACTION=PREPROCESS $(COMPILE) $(PREPROCESS) $(OUTPUT_OPTION) $<
o/$(MODE)/%.i: o/$(MODE)/%.c; @ACTION=PREPROCESS $(COMPILE) $(PREPROCESS) $(OUTPUT_OPTION) $<
o/$(MODE)/%.h: %.c; @ACTION=AMALGAMATE $(COMPILE) $(PREPROCESS) $(OUTPUT_OPTION) -fdirectives-only -P $<
o/$(MODE)/%.h: o/$(MODE)/%.c; @ACTION=AMALGAMATE $(COMPILE) $(PREPROCESS) $(OUTPUT_OPTION) -fdirectives-only -P $<
o/$(MODE)/%.o: %.S; @ACTION=OBJECTIFY.S $(COMPILE) $(OBJECTIFY.S) $(OUTPUT_OPTION) $<
o/$(MODE)/%.o: o/$(MODE)/%.S; @ACTION=OBJECTIFY.S $(COMPILE) $(OBJECTIFY.S) $(OUTPUT_OPTION) $<
o/$(MODE)/%.s: %.i; @ACTION=COMPILE.i $(COMPILE) $(COMPILE.i) $(OUTPUT_OPTION) $<
o/$(MODE)/%.s: o/$(MODE)/%.i; @ACTION=COMPILE.i $(COMPILE) $(COMPILE.i) $(OUTPUT_OPTION) $<
o/$(MODE)/%.o: %.cc; @ACTION=OBJECTIFY.cxx $(COMPILE) $(OBJECTIFY.cxx) $(OUTPUT_OPTION) $<
o/$(MODE)/%.o: o/$(MODE)/%.cc; @ACTION=OBJECTIFY.cxx $(COMPILE) $(OBJECTIFY.cxx) $(OUTPUT_OPTION) $<
o/$(MODE)/%.lds: %.lds; @ACTION=PREPROCESS $(COMPILE) $(PREPROCESS.lds) $(OUTPUT_OPTION) $<
o/$(MODE)/%.h.ok: %.h; @ACTION=CHECK.h $(COMPILE) $(COMPILE.c) -x c -g0 -o $@ $<
o/$(MODE)/%.h.okk: %.h; @ACTION=CHECK.h $(COMPILE) $(COMPILE.cxx) -x c++ -g0 -o $@ $<
o/$(MODE)/%.o: %.greg.c; @ACTION=OBJECTIFY.greg $(COMPILE) $(OBJECTIFY.greg.c) $(OUTPUT_OPTION) $<
o/$(MODE)/%.greg.o: %.greg.c; @ACTION=OBJECTIFY.greg $(COMPILE) $(OBJECTIFY.greg.c) $(OUTPUT_OPTION) $<
o/$(MODE)/%.ansi.o: %.ansi.c; @ACTION=OBJECTIFY.ansi $(COMPILE) $(OBJECTIFY.ansi.c) $(OUTPUT_OPTION) $<
o/$(MODE)/%.ansi.o: %.c; @ACTION=OBJECTIFY.ansi $(COMPILE) $(OBJECTIFY.ansi.c) $(OUTPUT_OPTION) $<
o/$(MODE)/%.c99.o: %.c99.c; @ACTION=OBJECTIFY.c99 $(COMPILE) $(OBJECTIFY.c99.c) $(OUTPUT_OPTION) $<
o/$(MODE)/%.c11.o: %.c11.c; @ACTION=OBJECTIFY.c11 $(COMPILE) $(OBJECTIFY.c11.c) $(OUTPUT_OPTION) $<
o/$(MODE)/%.c2x.o: %.c2x.c; @ACTION=OBJECTIFY.c2x $(COMPILE) $(OBJECTIFY.c2x.c) $(OUTPUT_OPTION) $<
o/$(MODE)/%.initabi.o: %.initabi.c; @ACTION=OBJECTIFY.init $(COMPILE) $(OBJECTIFY.initabi.c) $(OUTPUT_OPTION) $<
o/$(MODE)/%.ncabi.o: %.ncabi.c; @ACTION=OBJECTIFY.nc $(COMPILE) $(OBJECTIFY.ncabi.c) $(OUTPUT_OPTION) $<
o/$(MODE)/%.real.o: %.c; @ACTION=OBJECTIFY.real $(COMPILE) $(OBJECTIFY.real.c) $(OUTPUT_OPTION) $<
o/$(MODE)/%.runs: o/$(MODE)/%; @ACTION=CHECK.runs TARGET=$< build/runcom $< $(TESTARGS) && touch $@
o/$(MODE)/%.pkg:; @build/package $(OUTPUT_OPTION) $(addprefix -d,$(filter %.pkg,$^)) $(filter %.o,$^)
o/$(MODE)/%.zip.o: %; @build/zipobj $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/%-gcc.asm: %.c; @ACTION=OBJECTIFY.c $(COMPILE) $(OBJECTIFY.c) -S -g0 $(OUTPUT_OPTION) $<
o/$(MODE)/%-clang.asm: CC = $(CLANG)
o/$(MODE)/%-clang.asm: %.c; @ACTION=OBJECTIFY.c $(COMPILE) $(OBJECTIFY.c) -S -g0 $(OUTPUT_OPTION) $< || echo / need $(CLANG) >$@
o/$(MODE)/%-gcc.asm: %.f; @ACTION=OBJECTIFY.f $(COMPILE) $(OBJECTIFY.f) -S -g0 $(OUTPUT_OPTION) $<
o/$(MODE)/%-clang.asm: CC = $(CLANG)
o/$(MODE)/%-clang.asm: %.f; @ACTION=OBJECTIFY.f $(COMPILE) $(OBJECTIFY.f) -S -g0 $(OUTPUT_OPTION) $< || echo / need $(CLANG) >$@
o/$(MODE)/%-gcc.asm: %.F; @ACTION=OBJECTIFY.F $(COMPILE) $(OBJECTIFY.F) -S -g0 $(OUTPUT_OPTION) $<
o/$(MODE)/%-clang.asm: CC = $(CLANG)
o/$(MODE)/%-clang.asm: %.F; @ACTION=OBJECTIFY.F $(COMPILE) $(OBJECTIFY.F) -S -g0 $(OUTPUT_OPTION) $< || echo / need $(CLANG) >$@
