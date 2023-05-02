#include "libc/nt/codegen.h"
.imp	pdh,__imp_PdhAddEnglishCounterW,PdhAddEnglishCounterW,0

#ifdef __x86_64__
	.text.windows
PdhAddEnglishCounter:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_PdhAddEnglishCounterW(%rip),%rax
	jmp	__sysv2nt
	.endfn	PdhAddEnglishCounter,globl
	.previous
#endif
