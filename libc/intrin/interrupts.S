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
#include "libc/macros.internal.h"
#include "libc/runtime/pc.internal.h"

//	Code and data structures for bare metal interrupt handling.

#define ISR_STK_SZ 0x10000
#define ISR_STK_ALIGN 0x10
//	Interrupt stack to use for IRQs.  TODO.
#define IRQ_IST   1
//	Interrupt stack to use for CPU exceptions.
#define EXCEP_IST 2

//	Interrupt numbers to use for IRQs 0 & 8.  TODO: implement these!
#define IRQ0 0x20
#define IRQ8 0x28

	.init.start 100,_init_isr
	push	%rdi
	call	isr_init
	pop	%rdi
	.init.end 100,_init_isr

//	Interrupt service routines for CPU exceptions 0—31.
	i = 31
	.rept	30
	push	%rsi			# preserve rsi
	mov	$i,%sil			# rsi = exception number
1:	jmp	1f			# 🦘
	i = i - 1
	.endr
__excep1_isr:
	push	%rsi
	mov	$1,%sil
1:	jmp	1f
__excep0_isr:
	push	%rsi
	xor	%esi,%esi
1:	test	$8,%esp			# if no error code was pushed,
	jnz	2f			# stuff our own
	pushq	(%rsp)
	orq	$-1,8(%rsp)
2:	movzbq	%sil,%rsi		# zero-extend the exception number
	push	%rcx			# preserve registers which we will
	push	%rdx			# use to call kprintf
	push	%r8
	mov	40(%rsp),%rcx		# edx:rcx = "caller" cs:rip
	mov	48(%rsp),%edx
	mov	32(%rsp),%r8		# r8 = error code
	push	%rax			# preserve other call-used registers
	push	%rdi
	push	%r9
	push	%r10
	push	%r11
	mov	%ss,%eax		# preserve ds, es, ss
	push	%rax
	mov	%ds,%eax
	push	%rax
	mov	%es,%eax
	push	%rax
	mov	$GDT_LONG_DATA,%eax	# ...& load ds, es, ss correctly
	mov	%eax,%ss
	mov	%eax,%ds
	mov	%eax,%es
	ezlea	.excep_msg,di		# stack should be 16-byte aligned now
	xor	%eax,%eax		# kprintf is variadic; remember to
					# pass no. of vector regs. used (= 0)
	call	kprintf			# print error message
	cli
9:	hlt
	jmp	9b
	/* TODO: link up with sigaction etc. */

//	Initialization code for setting up a Task State Segment (TSS) &
//	Interrupt Descriptor Table (IDT) in bare metal mode, to start
//	processing exceptions & asynchronous IRQs.
isr_init:
	testb	IsMetal()
	jz	9f
	ezlea	_tss+0x24,di		# fill up TSS; we already loaded
					# task register in ape/ape.S
	ezlea	_isr_stk_1+ISR_STK_SZ,ax
	and	$-ISR_STK_ALIGN,%al	# be paranoid & enforce correct
	stosq				# alignment of stack pointers
	ezlea	_isr_stk_2+ISR_STK_SZ,ax
	and	$-ISR_STK_ALIGN,%al
	stosq
	add	$0x66-0x34,%rdi
	movw	$_tss_iopb-_tss,%ax
	stosw
	lidt	_idtr			# load IDTR
	ezlea	_idt,di
	pushpop	32,%rcx			# fill IDT entries for CPU exceptions
	ezlea	__excep0_isr,dx
1:	mov	%edx,%eax
	stosw
	mov	%cs,%eax
	stosw
	mov	%rdx,%rax
//		   ┌P:present
//		   │┌DPL:privilege
//		   ││ ┌system segment (0)
//		   ││ │ ┌gate type (interrupt gate, i.e. disable IRQs)
//		   ││ │ │    ┌reserved
//		   ││ │ │    │   ┌IST:interrupt stack table
//		   │├┐│┌┴─┐┌─┴─┐┌┴┐
	mov	$0b1000111000000000|EXCEP_IST,%ax
	stosl
	shr	$32,%rax
	stosq
	add	$__excep1_isr-__excep0_isr,%rdx
	loop	1b
9:	ret

//	String constants.
	.rodata.str1.1
.excep_msg:
	.asciz	"CPU exception %d @ %#llx:%#llx err code %#llx\n"
	.previous

//	IDTR value.
	.rodata
_idtr:	.short	_idt_end-_idt-1
	.quad	_idt
	.endobj	_idtr,globl,hidden
	.balign	8
	.previous

	.bss

//	Space for the Task State Segment.
_tss:
	.space	0x68
_tss_iopb:
_tss_end:
	.endobj	_tss,globl,hidden
	.endobj	_tss_end,globl,hidden

//	Space for the Interrupt Descriptor Table.
_idt:	.space	(MAX(IRQ0,IRQ8)+8)*0x10
_idt_end:
	.endobj	_idt,globl,hidden
	.previous

//	Interrupt stacks.
	.lcomm	_isr_stk_1,ISR_STK_SZ
	.lcomm	_isr_stk_2,ISR_STK_SZ
