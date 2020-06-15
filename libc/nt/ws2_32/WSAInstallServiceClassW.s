.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSAInstallServiceClassW,WSAInstallServiceClassW,77

	.text.windows
WSAInstallServiceClass:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_WSAInstallServiceClassW(%rip)
	leave
	ret
	.endfn	WSAInstallServiceClass,globl
	.previous
