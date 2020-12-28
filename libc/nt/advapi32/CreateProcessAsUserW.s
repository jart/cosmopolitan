.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_CreateProcessAsUserW,CreateProcessAsUserW,0

	.text.windows
CreateProcessAsUser:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateProcessAsUserW(%rip),%rax
	jmp	__sysv2nt12
	.endfn	CreateProcessAsUser,globl
	.previous
