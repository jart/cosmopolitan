/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/dce.h"
#include "libc/macros.internal.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/prot.h"
.privileged

//	Opens executable in O_RDWR mode.
//
//	To avoid ETXTBSY we need to unmap the running executable first,
//	then open the file, and finally load the code back into memory.
//
//	@return	file descriptor
//	@note	only works on .com binary (not .com.dbg)
//	@note	only supports linux, freebsd, openbsd, and netbsd
_OpenExecutable:
	push	%rbp
	mov	%rsp,%rbp
	pushq	__NR_open(%rip)			# -0x08(%rbp)
	pushq	__NR_mmap(%rip)			# -0x10(%rbp)
	pushq	__NR_munmap(%rip)		# -0x18(%rbp)
	pushq	O_RDWR(%rip)			# -0x20(%rbp)
	pushq	MAP_ANONYMOUS(%rip)		# -0x28(%rbp)
	pushq	MAP_PRIVATE(%rip)		# -0x30(%rbp)
	pushq	MAP_FIXED(%rip)			# -0x38(%rbp)
	pushq	__NR_mprotect(%rip)		# -0x40(%rbp)
	pushq	O_RDONLY(%rip)			# -0x48(%rbp)
	push	%rbx				# code buffer
	push	%r12				# data buffer
	push	%r14				# filename
	push	%r15				# fd

//	Get filename.
	lea	program_executable_name(%rip),%r14

//	Allocate code buffer.
	mov	-0x10(%rbp),%eax		# __NR_mmap
	xor	%edi,%edi
	mov	$PAGESIZE,%esi
	mov	$PROT_READ|PROT_WRITE,%edx
	mov	-0x28(%rbp),%r10d		# MAP_ANONYMOUS
	or	-0x30(%rbp),%r10d		# MAP_PRIVATE
	mov	$-1,%r8
	mov	$0,%r9
	push	%r9				# openbsd:pad
	push	%r9				# openbsd:align
	syscall
	pop	%r9
	pop	%r9
	mov	%rax,%rbx

//	Allocate data buffer.
	mov	-0x10(%rbp),%eax		# __NR_mmap
	xor	%edi,%edi
	mov	$ape_ram_filesz,%esi
	mov	$PROT_READ|PROT_WRITE,%edx
	mov	-0x28(%rbp),%r10d		# MAP_ANONYMOUS
	or	-0x30(%rbp),%r10d		# MAP_PRIVATE
	mov	$-1,%r8
	mov	$0,%r9
	push	%r9				# openbsd:pad
	push	%r9				# openbsd:align
	syscall
	pop	%r9
	pop	%r9
	mov	%rax,%r12

//	Move data.
	mov	%r12,%rdi
	mov	$ape_ram_vaddr,%esi
	mov	$ape_ram_filesz,%ecx
	rep movsb

//	Move code.
	mov	%rbx,%rdi
	mov	$8f,%esi
	mov	$9f-8f,%ecx
	rep movsb

//	Change protection.
	mov	-0x40(%rbp),%eax		# __NR_mprotect
	mov	%rbx,%rdi
	mov	$PAGESIZE,%esi
	mov	$PROT_READ|PROT_EXEC,%edx
	syscall

	jmp	*%rbx

//	<LIMBO>

//	Unmap code segment.
8:	mov	-0x18(%rbp),%eax		# __NR_munmap
	mov	$ape_rom_vaddr,%edi
	mov	$ape_rom_filesz,%esi
	syscall

//	Unmap data segment.
	mov	-0x18(%rbp),%eax		# __NR_munmap
	mov	$ape_ram_vaddr,%edi
	mov	$ape_ram_filesz,%esi
	syscall

//	Open executable in read-write mode.
	mov	-0x08(%rbp),%eax		# __NR_open
	mov	%r14,%rdi
	mov	-0x20(%rbp),%esi		# O_RDWR
        clc                                     # clear carry flag
        syscall
        jc      .Lohno                          # bsd error
        cmp     $-4095,%eax
        jae     .Lohno                          # linux error
        jmp     .Lok

//      Open executable in read-only mode.
.Lohno: mov     -0x08(%rbp),%eax                # __NR_open
        mov     %r14,%rdi
        mov     -0x48(%rbp),%esi                # O_RDONLY
        syscall

.Lok:   mov	%eax,%r15d

//	Map code segment.
	mov	-0x10(%rbp),%eax		# __NR_mmap
	mov	$ape_rom_vaddr,%edi
	mov	$ape_rom_filesz,%esi
	mov	$PROT_READ|PROT_EXEC,%edx
	mov	-0x38(%rbp),%r10d		# MAP_FIXED
	or	-0x30(%rbp),%r10d		# MAP_PRIVATE
	mov	%r15d,%r8d
	mov	$ape_rom_offset,%r9d
	push	%r9				# openbsd:pad
	push	%r9				# openbsd:align
	syscall
	pop	%r9
	pop	%r9

//	Allocate data segment.
	mov	-0x10(%rbp),%eax		# __NR_mmap
	mov	$ape_ram_vaddr,%edi
	mov	$ape_ram_filesz,%esi
	mov	$PROT_READ|PROT_WRITE,%edx
	mov	-0x38(%rbp),%r10d		# MAP_FIXED
	or	-0x30(%rbp),%r10d		# MAP_PRIVATE
	or	-0x28(%rbp),%r10d		# MAP_ANONYMOUS
	mov	$-1,%r8
	mov	$0,%r9
	push	%r9				# openbsd:pad
	push	%r9				# openbsd:align
	syscall
	pop	%r9
	pop	%r9

//	Put data back.
	mov	$ape_ram_vaddr,%edi
	mov	%r12,%rsi
	mov	$ape_ram_filesz,%ecx
	rep movsb

//	Jump back.
	mov	$9f,%eax
	jmp	*%rax

//	</LIMBO>

//	Deallocate code buffer.
9:	mov	__NR_munmap,%eax
	mov	%rbx,%rdi
	mov	$PAGESIZE,%esi
	syscall

//	Deallocate data buffer.
	mov	__NR_munmap,%eax
	mov	%r12,%rdi
	mov	$ape_ram_filesz,%esi
	syscall

	mov	%r15d,%eax
	pop	%r15
	pop	%r14
	pop	%r12
	pop	%rbx
	leave
	ret
9:	.endfn	_OpenExecutable,globl

	.weak	ape_rom_vaddr
	.weak	ape_rom_filesz
	.weak	ape_rom_offset
	.weak	ape_ram_vaddr
	.weak	ape_ram_filesz
