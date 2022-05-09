.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_ReportEventW,ReportEventW,0

	.text.windows
ReportEvent:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ReportEventW(%rip),%rax
	jmp	__sysv2nt10
	.endfn	ReportEvent,globl
	.previous
