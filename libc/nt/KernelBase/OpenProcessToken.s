.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_OpenProcessToken,OpenProcessToken,1048

	.text.windows
OpenProcessToken:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_OpenProcessToken(%rip),%rax
	jmp	__sysv2nt
	.endfn	OpenProcessToken,globl
	.previous
