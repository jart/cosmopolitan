.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_GetAddrInfoExOverlappedResult,GetAddrInfoExOverlappedResult,30

	.text.windows
GetAddrInfoExOverlappedResult:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetAddrInfoExOverlappedResult(%rip)
	leave
	ret
	.endfn	GetAddrInfoExOverlappedResult,globl
	.previous
