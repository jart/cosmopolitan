.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSAEnumNameSpaceProvidersExW,WSAEnumNameSpaceProvidersExW,62

	.text.windows
WSAEnumNameSpaceProvidersEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAEnumNameSpaceProvidersExW(%rip),%rax
	jmp	__sysv2nt
	.endfn	WSAEnumNameSpaceProvidersEx,globl
	.previous
