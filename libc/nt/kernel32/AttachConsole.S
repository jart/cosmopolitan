#include "libc/nt/codegen.h"
.imp	kernel32,__imp_AttachConsole,AttachConsole,0

	.text.windows
AttachConsole:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_AttachConsole(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	AttachConsole,globl
	.previous
