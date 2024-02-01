#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set et ft=make ts=8 sw=8 fenc=utf-8 :vi ──────────────────────┘
#
# SYNOPSIS
#
#   Cosmopolitan Online Testing
#
# OVERVIEW
#
#   make test HOSTS="rhel5 windows mac freebsd openbsd wsl"
#
# DESCRIPTION
#
#   This code deploys and spawns testing infrastructure to a fleet of
#   virtual machines running the various supported platforms. This is
#   super trivial since αcτµαlly pδrταblε εxεcµταblε is an autonomous
#   format requiring one simple file copy. Latencies are outstanding.
#   Configuration is trivial using /etc/hosts and ~/.ssh/config.
#
# SEE ALSO
#
#   - tool/build/runit.c
#   - tool/build/runitd.c

.PRECIOUS: o/$(MODE)/%.com.ok
o/$(MODE)/%.com.ok: private .PLEDGE = stdio rpath wpath cpath proc fattr inet dns
o/$(MODE)/%.com.ok: private .UNVEIL += r:/etc/resolv.conf
o/$(MODE)/%.com.ok:				\
		o/$(MODE)/tool/build/runit.com	\
		o/$(MODE)/tool/build/runitd.com	\
		o/$(MODE)/%.com
	@$(COMPILE) -wATEST -tT$@ $^ $(HOSTS)

.PHONY:
o/tiny/tool/build/runit.com:
