.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetExitCodeProcess,GetExitCodeProcess,536

	.text.windows
GetExitCodeProcess:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetExitCodeProcess(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetExitCodeProcess,globl
	.previous
