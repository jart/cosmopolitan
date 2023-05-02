#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_sendto,sendto,20

#ifdef __x86_64__
	.text.windows
__sys_sendto_nt:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_sendto(%rip),%rax
	jmp	__sysv2nt6
	.endfn	__sys_sendto_nt,globl
	.previous
#endif
