.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetExitCodeThread,GetExitCodeThread,0

	.text.windows
GetExitCodeThread:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetExitCodeThread(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetExitCodeThread,globl
	.previous
