.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_TlsAlloc,TlsAlloc,0

	.text.windows
__TlsAlloc:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_TlsAlloc(%rip)
	leave
	ret
	.endfn	__TlsAlloc,globl
	.previous
