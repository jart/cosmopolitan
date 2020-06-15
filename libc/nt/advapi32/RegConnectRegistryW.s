.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegConnectRegistryW,RegConnectRegistryW,1608

	.text.windows
RegConnectRegistry:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegConnectRegistryW(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegConnectRegistry,globl
	.previous
