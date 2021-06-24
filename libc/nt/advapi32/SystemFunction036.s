.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_SystemFunction036,SystemFunction036,0

	.text.windows
RtlGenRandom:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SystemFunction036(%rip),%rax
	jmp	__sysv2nt
	.endfn	RtlGenRandom,globl
	.previous
