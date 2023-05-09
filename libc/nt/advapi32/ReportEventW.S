#include "libc/nt/codegen.h"
.imp	advapi32,__imp_ReportEventW,ReportEventW,0

	.text.windows
ReportEvent:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ReportEventW(%rip),%rax
	jmp	__sysv2nt10
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	ReportEvent,globl
	.previous
