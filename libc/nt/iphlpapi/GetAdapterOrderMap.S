#include "libc/nt/codegen.h"
.imp	iphlpapi,__imp_GetAdapterOrderMap,GetAdapterOrderMap

	.text.windows
	.ftrace1
GetAdapterOrderMap:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	sub	$32,%rsp
	call	*__imp_GetAdapterOrderMap(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	GetAdapterOrderMap,globl
	.previous
