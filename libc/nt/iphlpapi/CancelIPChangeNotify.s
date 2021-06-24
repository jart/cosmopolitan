.include "o/libc/nt/codegen.inc"
.imp	iphlpapi,__imp_CancelIPChangeNotify,CancelIPChangeNotify,0

	.text.windows
CancelIPChangeNotify:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_CancelIPChangeNotify(%rip)
	leave
	ret
	.endfn	CancelIPChangeNotify,globl
	.previous
