.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_TlsSetValue,TlsSetValue,0

	.text.windows
__TlsSetValue:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_TlsSetValue(%rip),%rax
	jmp	__sysv2nt
	.endfn	__TlsSetValue,globl
	.previous
