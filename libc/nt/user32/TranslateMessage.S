#include "libc/nt/codegen.h"
.imp	user32,__imp_TranslateMessage,TranslateMessage,2449

#ifdef __x86_64__
	.text.windows
TranslateMessage:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_TranslateMessage(%rip)
	leave
	ret
	.endfn	TranslateMessage,globl
	.previous
#endif
