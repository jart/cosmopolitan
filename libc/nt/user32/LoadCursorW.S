#include "libc/nt/codegen.h"
.imp	user32,__imp_LoadCursorW,LoadCursorW,2101

#ifdef __x86_64__
	.text.windows
LoadCursor:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_LoadCursorW(%rip),%rax
	jmp	__sysv2nt
	.endfn	LoadCursor,globl
	.previous
#endif
