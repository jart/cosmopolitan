.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_SetFileCompletionNotificationModes,SetFileCompletionNotificationModes,1318

	.text.windows
SetFileCompletionNotificationModes:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetFileCompletionNotificationModes(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetFileCompletionNotificationModes,globl
	.previous
