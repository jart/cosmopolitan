#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TOOL_RELEASE

RELEASE_BINS =								\
	o/$(MODE)/tool/release/bin/lua					\
	o/$(MODE)/tool/release/bin/make					\
	o/$(MODE)/tool/release/bin/zip					\
	o/$(MODE)/tool/release/bin/unzip

RELEASE_CHECKS =							\
	o/$(MODE)/tool/release/bin/lua.ok				\
	o/$(MODE)/tool/release/bin/make.ok				\
	o/$(MODE)/tool/release/bin/zip.ok				\
	o/$(MODE)/tool/release/bin/unzip.ok

o/$(MODE)/tool/release/bin/lua: o/$(MODE)/tool/lua/lua.dbg
	@mkdir -p $(@D)
	cp $< $@

o/$(MODE)/tool/release/bin/make: o/$(MODE)/third_party/make/make.dbg
	@mkdir -p $(@D)
	cp $< $@

o/$(MODE)/tool/release/bin/zip: o/$(MODE)/third_party/zip/zip.dbg
	@mkdir -p $(@D)
	cp $< $@

o/$(MODE)/tool/release/bin/unzip: o/$(MODE)/third_party/unzip/unzip.dbg
	@mkdir -p $(@D)
	cp $< $@

o/$(MODE)/tool/release/bin/lua.ok: o/$(MODE)/tool/release/bin/lua
	$< -v
	grep -q MZqFpD $<
	@touch $@

o/$(MODE)/tool/release/bin/make.ok: o/$(MODE)/tool/release/bin/make
	$< --version
	grep -q MZqFpD $<
	@touch $@

o/$(MODE)/tool/release/bin/zip.ok: o/$(MODE)/tool/release/bin/zip
	$< -v
	grep -q MZqFpD $<
	@touch $@

o/$(MODE)/tool/release/bin/unzip.ok: o/$(MODE)/tool/release/bin/unzip
	$< -v
	grep -q MZqFpD $<
	@touch $@

.PHONY: o/$(MODE)/tool/release
o/$(MODE)/tool/release:							\
		$(RELEASE_BINS)						\
		$(RELEASE_CHECKS)
