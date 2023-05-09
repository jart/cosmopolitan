#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_listen,listen,13

	.text.windows
__sys_listen_nt:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_listen(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	__sys_listen_nt,globl
	.previous
