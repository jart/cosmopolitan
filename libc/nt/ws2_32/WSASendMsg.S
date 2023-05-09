#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSASendMsg,WSASendMsg,98

	.text.windows
WSASendMsg:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSASendMsg(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	WSASendMsg,globl
	.previous
