.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegSetValueW,RegSetValueW,1683

	.text.windows
RegSetValue:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegSetValueW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	RegSetValue,globl
	.previous
