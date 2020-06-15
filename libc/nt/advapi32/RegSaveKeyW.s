.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegSaveKeyW,RegSaveKeyW,1676

	.text.windows
RegSaveKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegSaveKeyW(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegSaveKey,globl
	.previous
