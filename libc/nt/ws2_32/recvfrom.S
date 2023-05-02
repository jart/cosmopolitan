#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_recvfrom,recvfrom,17

#ifdef __x86_64__
	.text.windows
__sys_recvfrom_nt:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_recvfrom(%rip),%rax
	jmp	__sysv2nt6
	.endfn	__sys_recvfrom_nt,globl
	.previous
#endif
