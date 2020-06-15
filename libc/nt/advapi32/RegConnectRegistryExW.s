.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegConnectRegistryExW,RegConnectRegistryExW,1607

	.text.windows
RegConnectRegistryEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegConnectRegistryExW(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegConnectRegistryEx,globl
	.previous
