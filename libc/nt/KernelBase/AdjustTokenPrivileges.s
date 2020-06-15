.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_AdjustTokenPrivileges,AdjustTokenPrivileges,36

	.text.windows
AdjustTokenPrivileges:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_AdjustTokenPrivileges(%rip),%rax
	jmp	__sysv2nt6
	.endfn	AdjustTokenPrivileges,globl
	.previous
