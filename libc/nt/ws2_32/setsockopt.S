#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_setsockopt,setsockopt,21

#ifdef __x86_64__
	.text.windows
__sys_setsockopt_nt:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_setsockopt(%rip),%rax
	jmp	__sysv2nt6
	.endfn	__sys_setsockopt_nt,globl
	.previous
#endif
