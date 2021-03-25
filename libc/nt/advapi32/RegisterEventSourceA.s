.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegisterEventSourceA,RegisterEventSourceA,1686

	.text.windows
RegisterEventSourceA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegisterEventSourceA(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegisterEventSourceA,globl
	.previous
