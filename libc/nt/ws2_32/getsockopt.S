#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_getsockopt,getsockopt,7

#ifdef __x86_64__
	.text.windows
__sys_getsockopt_nt:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_getsockopt(%rip),%rax
	jmp	__sysv2nt6
	.endfn	__sys_getsockopt_nt,globl
	.previous
#endif
