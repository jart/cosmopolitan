#include "libc/nt/codegen.h"
.imp	advapi32,__imp_SystemFunction036,SystemFunction036,0

	.text.windows
RtlGenRandom:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SystemFunction036(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	RtlGenRandom,globl
	.previous
