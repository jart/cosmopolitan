/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ This is free and unencumbered software released into the public domain.      │
│                                                                              │
│ Anyone is free to copy, modify, publish, use, compile, sell, or              │
│ distribute this software, either in source code form or as a compiled        │
│ binary, for any purpose, commercial or non-commercial, and by any            │
│ means.                                                                       │
│                                                                              │
│ In jurisdictions that recognize copyright laws, the author or authors        │
│ of this software dedicate any and all copyright interest in the              │
│ software to the public domain. We make this dedication for the benefit       │
│ of the public at large and to the detriment of our heirs and                 │
│ successors. We intend this dedication to be an overt act of                  │
│ relinquishment in perpetuity of all present and future rights to this        │
│ software under copyright law.                                                │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,              │
│ EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF           │
│ MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.       │
│ IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR            │
│ OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        │
│ ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR        │
│ OTHER DEALINGS IN THE SOFTWARE.                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/dce.h"
#include "libc/irq/irq.internal.h"
#include "libc/macros.internal.h"
#include "libc/runtime/pc.internal.h"

	.init.start 320,_init_irq
	testb	IsMetal()
	jz	9f
	push	%rdi
	push	%rsi
	ezlea	_idt+IRQ0*0x10,di	# fill IDT entry for IRQ 0
	ezlea	_irq0_isr,dx
	mov	%edx,%eax
	stosw
	mov	%cs,%eax
	stosw
	mov	%rdx,%rax
	mov	$0b1000111000000000|IRQ_IST,%ax
	stosl
	shr	$32,%rax
	stosq
	call	_IrqHwInit		# bring up interrupt controller(s) &
					# enable interrupts
	pop	%rsi
	pop	%rdi
9:
	.init.end 320,_init_irq
_irq0_isr:
	push	%rax
	mov	$PIC_EOI,%al
	out	%al,$PIC1_CMD
	pop	%rax
	iretq
	.rodata
_irq:
	.endobj	_irq,globl
	.previous
