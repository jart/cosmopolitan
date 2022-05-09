.include "o/libc/nt/codegen.inc"
.imp	MsWSock,__imp_TransmitFile,TransmitFile,0

	.text.windows
TransmitFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_TransmitFile(%rip),%rax
	jmp	__sysv2nt8
	.endfn	TransmitFile,globl
	.previous
