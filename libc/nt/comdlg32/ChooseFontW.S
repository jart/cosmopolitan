#include "libc/nt/codegen.h"
.imp	comdlg32,__imp_ChooseFontW,ChooseFontW,105

#ifdef __x86_64__
	.text.windows
ChooseFont:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_ChooseFontW(%rip)
	leave
	ret
	.endfn	ChooseFont,globl
	.previous
#endif
