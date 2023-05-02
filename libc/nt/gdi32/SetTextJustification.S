#include "libc/nt/codegen.h"
.imp	gdi32,__imp_SetTextJustification,SetTextJustification,1913

#ifdef __x86_64__
	.text.windows
SetTextJustification:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetTextJustification(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetTextJustification,globl
	.previous
#endif
