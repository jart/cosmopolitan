#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSAGetServiceClassNameByClassIdW,WSAGetServiceClassNameByClassIdW,73

#ifdef __x86_64__
	.text.windows
WSAGetServiceClassNameByClassId:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAGetServiceClassNameByClassIdW(%rip),%rax
	jmp	__sysv2nt
	.endfn	WSAGetServiceClassNameByClassId,globl
	.previous
#endif
