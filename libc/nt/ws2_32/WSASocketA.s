.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSASocketA,WSASocketA,119

	.text.windows
WSASocketA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSASocketA(%rip),%rax
	jmp	__sysv2nt6
	.endfn	WSASocketA,globl
	.previous
