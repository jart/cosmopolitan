#include "libc/nt/codegen.h"
.imp	user32,__imp_RegisterRawInputDevices,RegisterRawInputDevices

	.text.windows
	.ftrace1
RegisterRawInputDevices:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_RegisterRawInputDevices(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	RegisterRawInputDevices,globl
	.previous
