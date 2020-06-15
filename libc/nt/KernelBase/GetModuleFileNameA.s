.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetModuleFileNameA,GetModuleFileNameA,599

	.text.windows
GetModuleFileNameA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetModuleFileNameA(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetModuleFileNameA,globl
	.previous
