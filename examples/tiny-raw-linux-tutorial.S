/*-*- mode:asm; indent-tabs-mode:t; tab-width:8; coding:utf-8               -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
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
/	@see	libc/macros-cpp.inc forthe getstr macro definition
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
