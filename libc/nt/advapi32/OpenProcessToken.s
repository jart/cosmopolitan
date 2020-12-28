.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_OpenProcessToken,OpenProcessToken,0

	.text.windows
OpenProcessToken:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_OpenProcessToken(%rip),%rax
	jmp	__sysv2nt
	.endfn	OpenProcessToken,globl
	.previous
