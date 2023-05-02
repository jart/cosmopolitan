#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_ioctlsocket,ioctlsocket,10

#ifdef __x86_64__
	.text.windows
__sys_ioctlsocket_nt:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ioctlsocket(%rip),%rax
	jmp	__sysv2nt
	.endfn	__sys_ioctlsocket_nt,globl
	.previous
#endif
