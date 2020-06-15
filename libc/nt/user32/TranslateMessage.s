.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_TranslateMessage,TranslateMessage,2449

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
