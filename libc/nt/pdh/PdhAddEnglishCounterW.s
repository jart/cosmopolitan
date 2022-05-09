.include "o/libc/nt/codegen.inc"
.imp	pdh,__imp_PdhAddEnglishCounterW,PdhAddEnglishCounterW,0

	.text.windows
PdhAddEnglishCounter:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_PdhAddEnglishCounterW(%rip),%rax
	jmp	__sysv2nt
	.endfn	PdhAddEnglishCounter,globl
	.previous
