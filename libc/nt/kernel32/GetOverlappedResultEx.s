.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetOverlappedResultEx,GetOverlappedResultEx,0

	.text.windows
GetOverlappedResultEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetOverlappedResultEx(%rip),%rax
	jmp	__sysv2nt6
	.endfn	GetOverlappedResultEx,globl
	.previous
