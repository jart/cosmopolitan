#include "libc/nt/codegen.h"
.imp	user32,__imp_MessageBoxExW,MessageBoxExW,2165

#ifdef __x86_64__
	.text.windows
MessageBoxEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_MessageBoxExW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	MessageBoxEx,globl
	.previous
#endif
