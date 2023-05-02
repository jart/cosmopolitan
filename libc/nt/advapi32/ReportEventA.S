#include "libc/nt/codegen.h"
.imp	advapi32,__imp_ReportEventA,ReportEventA,0

#ifdef __x86_64__
	.text.windows
ReportEventA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ReportEventA(%rip),%rax
	jmp	__sysv2nt10
	.endfn	ReportEventA,globl
	.previous
#endif
