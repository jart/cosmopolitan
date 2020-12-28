.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_CreateProcessAsUserA,CreateProcessAsUserA,0

	.text.windows
CreateProcessAsUserA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateProcessAsUserA(%rip),%rax
	jmp	__sysv2nt12
	.endfn	CreateProcessAsUserA,globl
	.previous
