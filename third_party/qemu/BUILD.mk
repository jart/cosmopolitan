#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

o/third_party/qemu/qemu-aarch64:				\
		third_party/qemu/qemu-aarch64.gz
	@$(MKDIR) $(@D)
	@$(GZIP) $(ZFLAGS) -cd <$< >$@
	@$(CHMOD) 0755 $@
