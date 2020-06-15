.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegConnectRegistryExA,RegConnectRegistryExA,1606

	.text.windows
RegConnectRegistryExA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegConnectRegistryExA(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegConnectRegistryExA,globl
	.previous
