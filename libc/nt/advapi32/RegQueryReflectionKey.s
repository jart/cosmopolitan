.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegQueryReflectionKey,RegQueryReflectionKey,1663

	.text.windows
RegQueryReflectionKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegQueryReflectionKey(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegQueryReflectionKey,globl
	.previous
