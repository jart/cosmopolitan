#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_socket,socket,23

#ifdef __x86_64__
	.text.windows
__sys_socket_nt:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_socket(%rip),%rax
	jmp	__sysv2nt
	.endfn	__sys_socket_nt,globl
	.previous
#endif
