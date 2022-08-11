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
o/%.zip.o: o/%                     ; @$(COMPILE) -wAZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/%: o/$(MODE)/%.dbg       ; @$(COMPILE) -AOBJCOPY -T$@ $(OBJCOPY) -S -O binary $< $@
o/$(MODE)/%.o: %.s                 ; @$(COMPILE) -AOBJECTIFY.s $(OBJECTIFY.s) $(OUTPUT_OPTION) $<
o/$(MODE)/%.o: o/$(MODE)/%.s       ; @$(COMPILE) -AOBJECTIFY.s $(OBJECTIFY.s) $(OUTPUT_OPTION) $<
o/$(MODE)/%.s: %.S                 ; @$(COMPILE) -APREPROCESS $(PREPROCESS) $(OUTPUT_OPTION) $<
o/$(MODE)/%.s: o/$(MODE)/%.S       ; @$(COMPILE) -APREPROCESS $(PREPROCESS) $(OUTPUT_OPTION) $<
o/$(MODE)/%.o: %.f                 ; @$(COMPILE) -AOBJECTIFY.f $(OBJECTIFY.f) $(OUTPUT_OPTION) $<
o/$(MODE)/%.o: %.F                 ; @$(COMPILE) -AOBJECTIFY.F $(OBJECTIFY.F) $(OUTPUT_OPTION) $<
o/$(MODE)/%.ss: %.c                ; @$(COMPILE) -ACOMPILE.c $(COMPILE.c) $(OUTPUT_OPTION) $<
o/$(MODE)/%.ss: o/$(MODE)/%.c      ; @$(COMPILE) -AOBJECTIFY.s $(COMPILE.c) $(OUTPUT_OPTION) $<
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

o/$(MODE)/%.runs: o/$(MODE)/%      ; @$(COMPILE) -ACHECK -tT$@ $< $(TESTARGS)
o/$(MODE)/%.zip.o: %               ; @$(COMPILE) -wAZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<
o/$(MODE)/%-gcc.asm: %.c           ; @$(COMPILE) -AOBJECTIFY.c $(OBJECTIFY.c) -S -g0 $(OUTPUT_OPTION) $<
o/$(MODE)/%-gcc.asm: %.cc          ; @$(COMPILE) -AOBJECTIFY.c $(OBJECTIFY.cxx) -S -g0 $(OUTPUT_OPTION) $<
o/$(MODE)/%-clang.asm: %.c         ; @$(COMPILE) -AOBJECTIFY.c $(OBJECTIFY.c) -S -g0 $(OUTPUT_OPTION) $<
o/$(MODE)/%-clang.asm: %.cc        ; @$(COMPILE) -AOBJECTIFY.c $(OBJECTIFY.cxx) -S -g0 $(OUTPUT_OPTION) $<
o/$(MODE)/%-clang.asm: CC = $(CLANG)

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
	$(file >$(TMPDIR)/$(subst /,_,$@).args,$(filter %.o,$^))
	@$(COMPILE) -APACKAGE -wT$@ $(PKG) $(OUTPUT_OPTION) $(addprefix -d,$(filter %.pkg,$^)) @$(TMPDIR)/$(subst /,_,$@)

o/$(MODE)/%.pkg:
	$(file >$(TMPDIR)/$(subst /,_,$@),$(filter %.o,$^))
	@$(COMPILE) -APACKAGE -wT$@ $(PKG) $(OUTPUT_OPTION) $(addprefix -d,$(filter %.pkg,$^)) @$(TMPDIR)/$(subst /,_,$@)

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

# TODO(jart): find a way to generate dependencies
#             or alternatively disable sandboxing
o/%.h.ok: %.h
	@$(COMPILE) -wACHECK.h -T$@ build/bootstrap/touch.com $@
#	@$(COMPILE) -ACHECK.h $(COMPILE.c) -xc -g0 -o $@ $<
o/$(MODE)/%.h.ok: %.h
	@$(COMPILE) -wACHECK.h -T$@ build/bootstrap/touch.com $@
#	@$(COMPILE) -ACHECK.h $(COMPILE.c) -xc -g0 -o $@ $<
o/$(MODE)/%.hh.ok: %.hh
	@$(COMPILE) -wACHECK.h -T$@ build/bootstrap/touch.com $@
#	@$(COMPILE) -ACHECK.h $(COMPILE.cxx) -xc++ -g0 -o $@ $<
o/%.okk: %
	@$(COMPILE) -wACHECK.h -T$@ build/bootstrap/touch.com $@
#	@$(COMPILE) -ACHECK.h $(COMPILE.cxx) -xc++ -g0 -o $@ $<
o/$(MODE)/%.okk: %
	@$(COMPILE) -wACHECK.h -T$@ build/bootstrap/touch.com $@
#	@$(COMPILE) -ACHECK.h $(COMPILE.cxx) -xc++ -g0 -o $@ $<
