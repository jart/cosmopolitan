.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_PrefetchVirtualMemory,PrefetchVirtualMemory,0

	.text.windows
PrefetchVirtualMemory:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_PrefetchVirtualMemory(%rip),%rax
	jmp	__sysv2nt
	.endfn	PrefetchVirtualMemory,globl
	.previous
