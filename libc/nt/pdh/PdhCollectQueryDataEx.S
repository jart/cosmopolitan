#include "libc/nt/codegen.h"
.imp	pdh,__imp_PdhCollectQueryDataEx,PdhCollectQueryDataEx,0

#ifdef __x86_64__
	.text.windows
PdhCollectQueryDataEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_PdhCollectQueryDataEx(%rip),%rax
	jmp	__sysv2nt
	.endfn	PdhCollectQueryDataEx,globl
	.previous
#endif
