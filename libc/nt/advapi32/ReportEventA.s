.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_ReportEventA,ReportEventA,1704

	.text.windows
ReportEventA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ReportEventA(%rip),%rax
	jmp	__sysv2nt10
	.endfn	ReportEventA,globl
	.previous
