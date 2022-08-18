.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetVolumeInformationByHandleW,GetVolumeInformationByHandleW,0

	.text.windows
GetVolumeInformationByHandle:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetVolumeInformationByHandleW(%rip),%rax
	jmp	__sysv2nt8
	.endfn	GetVolumeInformationByHandle,globl
	.previous
