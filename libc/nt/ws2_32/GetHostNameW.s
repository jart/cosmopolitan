.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_GetHostNameW,GetHostNameW,33

	.text.windows
GetHostName:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetHostNameW(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetHostName,globl
	.previous
