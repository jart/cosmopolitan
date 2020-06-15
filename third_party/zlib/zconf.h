#ifndef COSMOPOLITAN_THIRD_PARTY_ZLIB_ZCONF_H_
#define COSMOPOLITAN_THIRD_PARTY_ZLIB_ZCONF_H_

#define STDC
#define STDC99
#define MAX_MEM_LEVEL 9
#define DEF_MEM_LEVEL 8
#define MAX_WBITS 15 /* 32K LZ77 window */

#if !(__ASSEMBLER__ + __LINKER__ + 0)

typedef unsigned char Byte;
typedef unsigned int uInt;   /* 16 bits or more */
typedef unsigned long uLong; /* 32 bits or more */
typedef Byte Bytef;
typedef char charf;
typedef int intf;
typedef uInt uIntf;
typedef uLong uLongf;
typedef void const *voidpc;
typedef void *voidpf;
typedef void *voidp;

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_ZLIB_ZCONF_H_ */
