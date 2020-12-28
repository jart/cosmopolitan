.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_AdjustTokenPrivileges,AdjustTokenPrivileges,0

	.text.windows
AdjustTokenPrivileges:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_AdjustTokenPrivileges(%rip),%rax
	jmp	__sysv2nt6
	.endfn	AdjustTokenPrivileges,globl
	.previous
