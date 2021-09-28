.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetVolumePathNameA,GetVolumePathNameA,807

	.text.windows
GetVolumePathNameA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetVolumePathNameA(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetVolumePathNameA,globl
	.previous
