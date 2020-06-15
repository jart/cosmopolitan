#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TOOL_CC

o/$(MODE)/tool/cc/c11.c: tool/cc/c11.l o/$(MODE)/third_party/lex/lex.com.dbg
	@mkdir -p $(dir $@)
	o/$(MODE)/third_party/lex/lex.com.dbg -o $@ $<

.PHONY: o/$(MODE)/tool/cc
o/$(MODE)/tool/cc:
