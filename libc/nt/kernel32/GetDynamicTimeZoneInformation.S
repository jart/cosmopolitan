#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetDynamicTimeZoneInformation,GetDynamicTimeZoneInformation

	.text.windows
	.ftrace1
GetDynamicTimeZoneInformation:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetDynamicTimeZoneInformation(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	GetDynamicTimeZoneInformation,globl
	.previous
