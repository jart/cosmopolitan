#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_recv,recv,16

#ifdef __x86_64__
	.text.windows
__sys_recv_nt:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_recv(%rip),%rax
	jmp	__sysv2nt
	.endfn	__sys_recv_nt,globl
	.previous
#endif
