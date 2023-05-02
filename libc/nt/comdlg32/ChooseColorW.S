#include "libc/nt/codegen.h"
.imp	comdlg32,__imp_ChooseColorW,ChooseColorW,103

#ifdef __x86_64__
	.text.windows
ChooseColor:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_ChooseColorW(%rip)
	leave
	ret
	.endfn	ChooseColor,globl
	.previous
#endif
