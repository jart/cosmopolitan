.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_PrefetchVirtualMemory,PrefetchVirtualMemory,1222

	.text.windows
PrefetchVirtualMemory:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_PrefetchVirtualMemory(%rip),%rax
	jmp	__sysv2nt
	.endfn	PrefetchVirtualMemory,globl
	.previous
