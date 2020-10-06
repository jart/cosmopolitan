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
o/%.o: %.s; @TARGET=$@ build/assemble $(OBJECTIFY.s) $(OUTPUT_OPTION) $<
o/%.o: o/%.s; @TARGET=$@ build/assemble $(OBJECTIFY.s) $(OUTPUT_OPTION) $<
o/%.s: %.S; @ACTION=PREPROCESS build/compile $(PREPROCESS) $(OUTPUT_OPTION) $<
o/%.s: o/%.S; @ACTION=PREPROCESS build/compile $(PREPROCESS) $(OUTPUT_OPTION) $<
o/%.i: %.S; @ACTION=PREPROCESS build/compile $(PREPROCESS) $(OUTPUT_OPTION) $<
o/%.o: %.S; @ACTION=OBJECTIFY.S build/compile $(OBJECTIFY.S) $(OUTPUT_OPTION) $<
o/%.o: o/%.S; @ACTION=OBJECTIFY.S build/compile $(OBJECTIFY.S) $(OUTPUT_OPTION) $<
o/%.s: %.i; @ACTION=COMPILE.i build/compile $(COMPILE.i) $(OUTPUT_OPTION) $<
o/%.s: o/%.i; @ACTION=COMPILE.i build/compile $(COMPILE.i) $(OUTPUT_OPTION) $<
o/%.o: %.cc; @ACTION=OBJECTIFY.cxx build/compile $(OBJECTIFY.cxx) $(OUTPUT_OPTION) $<
o/%.o: o/%.cc; @ACTION=OBJECTIFY.cxx build/compile $(OBJECTIFY.cxx) $(OUTPUT_OPTION) $<
o/%.lds: %.lds; @ACTION=PREPROCESS build/compile $(PREPROCESS.lds) $(OUTPUT_OPTION) $<
o/%.inc: %.h; @ACTION=PREPROCESS build/compile $(PREPROCESS) $(OUTPUT_OPTION) -D__ASSEMBLER__ -P $<
o/%.pkg:; @build/package $(OUTPUT_OPTION) $(addprefix -d,$(filter %.pkg,$^)) $(filter %.o,$^)
o/%.h.ok: %.h; @ACTION=CHECK.h build/compile $(COMPILE.c) -x c -g0 -o $@ $<
o/%.h.okk: %.h; @ACTION=CHECK.h build/compile $(COMPILE.cxx) -x c++ -g0 -o $@ $<
o/%.greg.o: %.greg.c; @ACTION=OBJECTIFY.greg build/compile $(OBJECTIFY.greg.c) $(OUTPUT_OPTION) $<
o/%.zip.o: o/%; @build/zipobj $(OUTPUT_OPTION) $<

o/$(MODE)/%.a:; @$(ARCHIVE) $@ $^
o/$(MODE)/%: o/$(MODE)/%.dbg; @ACTION=OBJCOPY TARGET=$@ build/do $(OBJCOPY) -SO binary $< $@
o/$(MODE)/%.o: %.s; @TARGET=$@ build/assemble $(OBJECTIFY.s) $(OUTPUT_OPTION) $<
o/$(MODE)/%.o: o/$(MODE)/%.s; @TARGET=$@ build/assemble $(OBJECTIFY.s) $(OUTPUT_OPTION) $<
o/$(MODE)/%.s: %.S; @ACTION=PREPROCESS build/compile $(PREPROCESS) $(OUTPUT_OPTION) $<
o/$(MODE)/%.s: o/$(MODE)/%.S; @ACTION=PREPROCESS build/compile $(PREPROCESS) $(OUTPUT_OPTION) $<
o/$(MODE)/%.o: %.c; @ACTION=OBJECTIFY.c build/compile $(OBJECTIFY.c) $(OUTPUT_OPTION) $<
o/$(MODE)/%.o: %.f; @ACTION=OBJECTIFY.f build/compile $(OBJECTIFY.f) $(OUTPUT_OPTION) $<
o/$(MODE)/%.o: %.F; @ACTION=OBJECTIFY.F build/compile $(OBJECTIFY.F) $(OUTPUT_OPTION) $<
o/$(MODE)/%.o: o/$(MODE)/%.c; @ACTION=OBJECTIFY.c build/compile $(OBJECTIFY.c) $(OUTPUT_OPTION) $<
o/$(MODE)/%.ss: %.c; @ACTION=COMPILE.c build/compile $(COMPILE.c) $(OUTPUT_OPTION) $<
o/$(MODE)/%.ss: o/$(MODE)/%.c; @ACTION=OBJECTIFY.s build/compile $(COMPILE.c) $(OUTPUT_OPTION) $<
o/$(MODE)/%.i: %.S; @ACTION=PREPROCESS build/compile $(PREPROCESS) $(OUTPUT_OPTION) $<
o/$(MODE)/%.i: %.c; @ACTION=PREPROCESS build/compile $(PREPROCESS) $(OUTPUT_OPTION) $<
o/$(MODE)/%.i: %.cc; @ACTION=PREPROCESS build/compile $(PREPROCESS) $(OUTPUT_OPTION) $<
o/$(MODE)/%.i: o/$(MODE)/%.c; @ACTION=PREPROCESS build/compile $(PREPROCESS) $(OUTPUT_OPTION) $<
o/$(MODE)/%.h: %.c; @ACTION=AMALGAMATE build/compile $(PREPROCESS) $(OUTPUT_OPTION) -fdirectives-only -P $<
o/$(MODE)/%.h: o/$(MODE)/%.c; @ACTION=AMALGAMATE build/compile $(PREPROCESS) $(OUTPUT_OPTION) -fdirectives-only -P $<
o/$(MODE)/%.o: %.S; @ACTION=OBJECTIFY.S build/compile $(OBJECTIFY.S) $(OUTPUT_OPTION) $<
o/$(MODE)/%.o: o/$(MODE)/%.S; @ACTION=OBJECTIFY.S build/compile $(OBJECTIFY.S) $(OUTPUT_OPTION) $<
o/$(MODE)/%.s: %.i; @ACTION=COMPILE.i build/compile $(COMPILE.i) $(OUTPUT_OPTION) $<
o/$(MODE)/%.s: o/$(MODE)/%.i; @ACTION=COMPILE.i build/compile $(COMPILE.i) $(OUTPUT_OPTION) $<
o/$(MODE)/%.o: %.cc; @ACTION=OBJECTIFY.cxx build/compile $(OBJECTIFY.cxx) $(OUTPUT_OPTION) $<
o/$(MODE)/%.o: o/$(MODE)/%.cc; @ACTION=OBJECTIFY.cxx build/compile $(OBJECTIFY.cxx) $(OUTPUT_OPTION) $<
o/$(MODE)/%.lds: %.lds; @ACTION=PREPROCESS build/compile $(PREPROCESS.lds) $(OUTPUT_OPTION) $<
o/$(MODE)/%.h.ok: %.h; @ACTION=CHECK.h build/compile $(COMPILE.c) -x c -g0 -o $@ $<
o/$(MODE)/%.h.okk: %.h; @ACTION=CHECK.h build/compile $(COMPILE.cxx) -x c++ -g0 -o $@ $<
o/$(MODE)/%.o: %.greg.c; @ACTION=OBJECTIFY.greg build/compile $(OBJECTIFY.greg.c) $(OUTPUT_OPTION) $<
o/$(MODE)/%.greg.o: %.greg.c; @ACTION=OBJECTIFY.greg build/compile $(OBJECTIFY.greg.c) $(OUTPUT_OPTION) $<
o/$(MODE)/%.ansi.o: %.ansi.c; @ACTION=OBJECTIFY.ansi build/compile $(OBJECTIFY.ansi.c) $(OUTPUT_OPTION) $<
o/$(MODE)/%.ansi.o: %.c; @ACTION=OBJECTIFY.ansi build/compile $(OBJECTIFY.ansi.c) $(OUTPUT_OPTION) $<
o/$(MODE)/%.c99.o: %.c99.c; @ACTION=OBJECTIFY.c99 build/compile $(OBJECTIFY.c99.c) $(OUTPUT_OPTION) $<
o/$(MODE)/%.c11.o: %.c11.c; @ACTION=OBJECTIFY.c11 build/compile $(OBJECTIFY.c11.c) $(OUTPUT_OPTION) $<
o/$(MODE)/%.c2x.o: %.c2x.c; @ACTION=OBJECTIFY.c2x build/compile $(OBJECTIFY.c2x.c) $(OUTPUT_OPTION) $<
o/$(MODE)/%.initabi.o: %.initabi.c; @ACTION=OBJECTIFY.init build/compile $(OBJECTIFY.initabi.c) $(OUTPUT_OPTION) $<
o/$(MODE)/%.ncabi.o: %.ncabi.c; @ACTION=OBJECTIFY.nc build/compile $(OBJECTIFY.ncabi.c) $(OUTPUT_OPTION) $<
o/$(MODE)/%.real.o: %.c; @ACTION=OBJECTIFY.real build/compile $(OBJECTIFY.real.c) $(OUTPUT_OPTION) $<
o/$(MODE)/%.runs: o/$(MODE)/%; @ACTION=CHECK.runs TARGET=$< build/runcom $< $(TESTARGS) && touch $@
o/$(MODE)/%.pkg:; @build/package $(OUTPUT_OPTION) $(addprefix -d,$(filter %.pkg,$^)) $(filter %.o,$^)
o/$(MODE)/%.zip.o: %; @build/zipobj $(OUTPUT_OPTION) $<

o/$(MODE)/%-gcc.asm: %.c; @ACTION=OBJECTIFY.c build/compile $(OBJECTIFY.c) -S -g0 $(OUTPUT_OPTION) $<
o/$(MODE)/%-clang.asm: CC = $(CLANG)
o/$(MODE)/%-clang.asm: %.c; @ACTION=OBJECTIFY.c build/compile $(OBJECTIFY.c) -S -g0 $(OUTPUT_OPTION) $< || echo / need $(CLANG) >$@
o/$(MODE)/%-gcc.asm: %.f; @ACTION=OBJECTIFY.f build/compile $(OBJECTIFY.f) -S -g0 $(OUTPUT_OPTION) $<
o/$(MODE)/%-clang.asm: CC = $(CLANG)
o/$(MODE)/%-clang.asm: %.f; @ACTION=OBJECTIFY.f build/compile $(OBJECTIFY.f) -S -g0 $(OUTPUT_OPTION) $< || echo / need $(CLANG) >$@
o/$(MODE)/%-gcc.asm: %.F; @ACTION=OBJECTIFY.F build/compile $(OBJECTIFY.F) -S -g0 $(OUTPUT_OPTION) $<
o/$(MODE)/%-clang.asm: CC = $(CLANG)
o/$(MODE)/%-clang.asm: %.F; @ACTION=OBJECTIFY.F build/compile $(OBJECTIFY.F) -S -g0 $(OUTPUT_OPTION) $< || echo / need $(CLANG) >$@

# ragel state machine compiler
.PRECIOUS: build/bootstrap/%.c.gz
o/$(MODE)/%.c: %.rl build/bootstrap/%.c.gz
	@mkdir -p $(dir $@)
	@$(GZ) $(ZFLAGS) -dc $(<:%.rl=build/bootstrap/%.c.gz) >$@
	-@ACTION=RAGEL build/do $(RAGEL) $(RAGELFLAGS) $(OUTPUT_OPTION) $<
build/bootstrap/%.c.gz: %.rl
	@mkdir -p $(dir $@)
	@$(RAGEL) -o $(@:%.gz=%) $<
	@$(GZ) $(ZFLAGS) -f $(@:%.gz=%)
%.svgz: %.rl
	@$(RAGEL) -V -p $< | $(DOT) -Tsvg | $(GZ) $(ZFLAGS) >$@
