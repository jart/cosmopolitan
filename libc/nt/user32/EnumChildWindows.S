#include "libc/nt/codegen.h"
.imp	user32,__imp_EnumChildWindows,EnumChildWindows,1755

#ifdef __x86_64__
	.text.windows
EnumChildWindows:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_EnumChildWindows(%rip),%rax
	jmp	__sysv2nt
	.endfn	EnumChildWindows,globl
	.previous
#endif
