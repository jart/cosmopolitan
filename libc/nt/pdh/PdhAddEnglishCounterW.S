#include "libc/nt/codegen.h"
.imp	pdh,__imp_PdhAddEnglishCounterW,PdhAddEnglishCounterW,0

	.text.windows
PdhAddEnglishCounter:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_PdhAddEnglishCounterW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	PdhAddEnglishCounter,globl
	.previous
