.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GlobalAlloc,GlobalAlloc,0

	.text.windows
GlobalAlloc:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GlobalAlloc(%rip),%rax
	jmp	__sysv2nt
	.endfn	GlobalAlloc,globl
	.previous
