#include "libc/nt/codegen.h"
.imp	user32,__imp_LoadCursorW,LoadCursorW,2101

	.text.windows
LoadCursor:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_LoadCursorW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	LoadCursor,globl
	.previous
