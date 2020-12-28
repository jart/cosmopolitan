.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetExitCodeProcess,GetExitCodeProcess,0

	.text.windows
GetExitCodeProcess:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetExitCodeProcess(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetExitCodeProcess,globl
	.previous
