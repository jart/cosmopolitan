.include "o/libc/nt/codegen.inc"
.imp	iphlpapi,__imp_GetTcpTable2,GetTcpTable2,0

	.text.windows
GetTcpTable2:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetTcpTable2(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetTcpTable2,globl
	.previous
