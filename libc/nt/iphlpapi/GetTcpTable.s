.include "o/libc/nt/codegen.inc"
.imp	iphlpapi,__imp_GetTcpTable,GetTcpTable,0

	.text.windows
GetTcpTable:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetTcpTable(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetTcpTable,globl
	.previous
