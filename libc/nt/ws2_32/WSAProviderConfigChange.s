.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSAProviderConfigChange,WSAProviderConfigChange,90

	.text.windows
WSAProviderConfigChange:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAProviderConfigChange(%rip),%rax
	jmp	__sysv2nt
	.endfn	WSAProviderConfigChange,globl
	.previous
