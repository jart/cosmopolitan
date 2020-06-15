.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetLogicalDrives,GetLogicalDrives,589

	.text.windows
GetLogicalDrives:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_GetLogicalDrives(%rip)
	leave
	ret
	.endfn	GetLogicalDrives,globl
	.previous
