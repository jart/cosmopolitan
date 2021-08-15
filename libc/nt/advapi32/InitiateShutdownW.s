.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_InitiateShutdownW,InitiateShutdownW,1403

	.text.windows
InitiateShutdown:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_InitiateShutdownW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	InitiateShutdown,globl
	.previous
