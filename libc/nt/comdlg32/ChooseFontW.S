#include "libc/nt/codegen.h"
.imp	comdlg32,__imp_ChooseFontW,ChooseFontW,105

	.text.windows
ChooseFont:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_ChooseFontW(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	ChooseFont,globl
	.previous
