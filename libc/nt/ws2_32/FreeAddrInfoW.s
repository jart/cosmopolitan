.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_FreeAddrInfoW,FreeAddrInfoW,27

	.text.windows
FreeAddrInfo:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_FreeAddrInfoW(%rip)
	leave
	ret
	.endfn	FreeAddrInfo,globl
	.previous
