#include "libc/nt/codegen.h"
.imp	API-MS-Win-Core-Synch-l1-2-0,__imp_WakeByAddressAll,WakeByAddressAll

	.text.windows
	.ftrace1
WakeByAddressAll:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_WakeByAddressAll(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	WakeByAddressAll,globl
	.previous
