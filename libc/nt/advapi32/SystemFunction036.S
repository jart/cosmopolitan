#include "libc/nt/codegen.h"
.imp	advapi32,__imp_SystemFunction036,SystemFunction036,0

#ifdef __x86_64__
	.text.windows
RtlGenRandom:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SystemFunction036(%rip),%rax
	jmp	__sysv2nt
	.endfn	RtlGenRandom,globl
	.previous
#endif
