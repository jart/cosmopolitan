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

/	Raw Linux Assembly Binary Tutorial
/	
/	i.e. how to not use cosmopolitan runtimes at all
/	     cosmopolitan basically abstracts this
/	     except for all major platforms
/	
/	make o//examples/raw-linux-hello.elf
/	o//examples/raw-linux-hello.elf		# about 6kb
/	
/	Next consider doing it in C with fancy build tuning
/	
/	make -j8 -O \
/	  MODE=tiny \
/	  LDFLAGS=-s \
/	  CPPFLAGS=-DSUPPORT_VECTOR=0b00000001 \
/	  o/tiny/examples/hello2.elf
/	o/tiny/examples/hello2.elf		# about 8kb
/
/	@noreturn
_start:	mov	$12,%rdx			# arg no. 3 is length
	getstr	"hello world\n",%rsi,%esi	# arg no. 2 is memory
	mov	$1,%edi				# arg no. 1 is stdout
	mov	$1,%eax				# write()
	syscall					# see libc/sysv/syscalls.sh
	mov	$0,%edi				# arg no. 1 is success status
	mov	$0xE7,%eax			# exit_group()
	syscall
	.endfn	_start,globl
