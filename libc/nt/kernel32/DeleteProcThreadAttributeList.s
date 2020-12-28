.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_DeleteProcThreadAttributeList,DeleteProcThreadAttributeList,0

	.text.windows
DeleteProcThreadAttributeList:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_DeleteProcThreadAttributeList(%rip)
	leave
	ret
	.endfn	DeleteProcThreadAttributeList,globl
	.previous
