.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegQueryValueExW,RegQueryValueExW,0

	.text.windows
RegQueryValueEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegQueryValueExW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	RegQueryValueEx,globl
	.previous
