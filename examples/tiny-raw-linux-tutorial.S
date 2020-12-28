/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/macros.h"

/	Tiny Raw Linux Binary Tutorial
/	
/	i.e. how to not use cosmopolitan runtimes at all
/	     cosmopolitan basically abstracts this
/	     except for all major platforms
/	
/	  make o//examples/raw-linux-hello.elf
/	  o/examples/raw-linux-hello.elf # about 6kb
/	
/	Next try C but with fancy build tuning
/	
/	  make -j8 -O                             \
/	    MODE=tiny                             \
/	    LDFLAGS+=-s                           \
/	    CPPFLAGS+=-DIM_FEELING_NAUGHTY        \
/	    CPPFLAGS+=-DSUPPORT_VECTOR=0b00000001 \
/	    o/tiny/examples/hello2.elf
/	  o/tiny/examples/hello2.elf # about 8kb
/
/	@param	rsp is [n,argv₀..argvₙ₋₁,0,envp₀..,0,auxv₀..,0,..]
/	@see	also glibc static binaries which start at 800kb!!!
/	@see	also go where interfaces sadly disempower ld prune
/	@see	also the stl where bad linkage is due to tech debt
/	@note	libc/elf/elf.lds can be tinier with page align off
/	@note	gas is more powerful than nasm due to rms notation
/	@noreturn
_start:	mov	$12,%rdx			# arg no. 3 is length
	getstr	"hello world\n",%rsi,%esi	# arg no. 2 is memory
	mov	$1,%edi				# arg no. 1 is stdout
	mov	$1,%eax				# write()
	syscall					# libc/sysv/syscalls.sh
	mov	$0,%edi				# arg no. 1 is success status
	mov	$0xE7,%eax			# exit_group()
	syscall					# context switch
0:	rep nop					# basic blockading
	jmp	0b
	.endfn	_start,globl
	.source	__FILE__
