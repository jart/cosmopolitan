.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegConnectRegistryA,RegConnectRegistryA,1605

	.text.windows
RegConnectRegistryA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegConnectRegistryA(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegConnectRegistryA,globl
	.previous
