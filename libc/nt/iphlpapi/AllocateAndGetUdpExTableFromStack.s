.include "o/libc/nt/codegen.inc"
.imp	iphlpapi,__imp_AllocateAndGetUdpExTableFromStack,AllocateAndGetUdpExTableFromStack,0

	.text.windows
AllocateAndGetUdpExTableFromStack:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_AllocateAndGetUdpExTableFromStack(%rip),%rax
	jmp	__sysv2nt6
	.endfn	AllocateAndGetUdpExTableFromStack,globl
	.previous
