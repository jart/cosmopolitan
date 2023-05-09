#include "libc/nt/codegen.h"
.imp	kernel32,__imp_SetFileCompletionNotificationModes,SetFileCompletionNotificationModes,1318

	.text.windows
SetFileCompletionNotificationModes:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetFileCompletionNotificationModes(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	SetFileCompletionNotificationModes,globl
	.previous
