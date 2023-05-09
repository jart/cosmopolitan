#include "libc/nt/codegen.h"
.imp	user32,__imp_EnumChildWindows,EnumChildWindows,1755

	.text.windows
EnumChildWindows:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_EnumChildWindows(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	EnumChildWindows,globl
	.previous
