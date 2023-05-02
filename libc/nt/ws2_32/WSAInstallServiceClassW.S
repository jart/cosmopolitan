#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSAInstallServiceClassW,WSAInstallServiceClassW,77

#ifdef __x86_64__
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
#endif
