.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_FindVolumeClose,FindVolumeClose,383

	.text.windows
FindVolumeClose:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_FindVolumeClose(%rip)
	leave
	ret
	.endfn	FindVolumeClose,globl
	.previous
