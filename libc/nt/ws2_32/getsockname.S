#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_getsockname,getsockname,6

#ifdef __x86_64__
	.text.windows
__sys_getsockname_nt:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_getsockname(%rip),%rax
	jmp	__sysv2nt
	.endfn	__sys_getsockname_nt,globl
	.previous
#endif
