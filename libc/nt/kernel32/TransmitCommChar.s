.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_TransmitCommChar,TransmitCommChar,0

	.text.windows
TransmitCommChar:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_TransmitCommChar(%rip),%rax
	jmp	__sysv2nt
	.endfn	TransmitCommChar,globl
	.previous
