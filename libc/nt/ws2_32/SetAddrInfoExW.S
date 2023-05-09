#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_SetAddrInfoExW,SetAddrInfoExW,38

	.text.windows
SetAddrInfoEx:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetAddrInfoExW(%rip),%rax
	jmp	__sysv2nt12
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	SetAddrInfoEx,globl
	.previous
