#include "libc/nt/ntdllimport.h"
.ntimp	NtDeleteKey,NtDeleteKey

	.text.windows
	.ftrace1
NtDeleteKey:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_NtDeleteKey(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	NtDeleteKey,globl
	.previous
