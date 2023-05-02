#include "libc/nt/codegen.h"
.imp	kernel32,__imp_SetFileCompletionNotificationModes,SetFileCompletionNotificationModes,1318

#ifdef __x86_64__
	.text.windows
SetFileCompletionNotificationModes:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetFileCompletionNotificationModes(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetFileCompletionNotificationModes,globl
	.previous
#endif
