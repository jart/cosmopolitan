.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegisterEventSourceW,RegisterEventSourceW,1687

	.text.windows
RegisterEventSource:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegisterEventSourceW(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegisterEventSource,globl
	.previous
