#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_GetAddrInfoExCancel,GetAddrInfoExCancel

	.text.windows
	.ftrace1
GetAddrInfoExCancel:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetAddrInfoExCancel(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	GetAddrInfoExCancel,globl
	.previous
