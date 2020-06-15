.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSAEnumNameSpaceProvidersExA,WSAEnumNameSpaceProvidersExA,61

	.text.windows
WSAEnumNameSpaceProvidersExA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAEnumNameSpaceProvidersExA(%rip),%rax
	jmp	__sysv2nt
	.endfn	WSAEnumNameSpaceProvidersExA,globl
	.previous
