#include "libc/macros.h"

MyAsm:	.leafprologue
	call	MyPrint2
	.leafepilogue
	.endfn	MyAsm,globl
