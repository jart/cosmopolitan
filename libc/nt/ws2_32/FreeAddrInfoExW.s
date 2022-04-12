.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_FreeAddrInfoExW,FreeAddrInfoExW,26

	.text.windows
FreeAddrInfoEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_FreeAddrInfoExW(%rip)
	leave
	ret
	.endfn	FreeAddrInfoEx,globl
	.previous
