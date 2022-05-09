.include "o/libc/nt/codegen.inc"
.imp	pdh,__imp_PdhCollectQueryDataEx,PdhCollectQueryDataEx,0

	.text.windows
PdhCollectQueryDataEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_PdhCollectQueryDataEx(%rip),%rax
	jmp	__sysv2nt
	.endfn	PdhCollectQueryDataEx,globl
	.previous
