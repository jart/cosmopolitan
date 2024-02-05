#include "libc/nt/codegen.h"
.imp	iphlpapi,__imp_CancelIPChangeNotify,CancelIPChangeNotify

	.text.windows
	.ftrace1
CancelIPChangeNotify:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_CancelIPChangeNotify(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	CancelIPChangeNotify,globl
	.previous
