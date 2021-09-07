.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetOverlappedResult,GetOverlappedResult,0

	.text.windows
GetOverlappedResult:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetOverlappedResult(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetOverlappedResult,globl
	.previous
