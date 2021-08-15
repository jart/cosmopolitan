.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_InitiateShutdownA,InitiateShutdownA,1402

	.text.windows
InitiateShutdownA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_InitiateShutdownA(%rip),%rax
	jmp	__sysv2nt6
	.endfn	InitiateShutdownA,globl
	.previous
