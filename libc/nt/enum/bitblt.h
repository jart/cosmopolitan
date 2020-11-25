#ifndef COSMOPOLITAN_LIBC_NT_ENUM_BITBLT_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_BITBLT_H_

#define kNtSrccopy     0x00CC0020u /* src */
#define kNtSrcpaint    0x00EE0086u /* src | dst */
#define kNtSrcand      0x008800C6u /* src & dst */
#define kNtSrcinvert   0x00660046u /* src ^ dst */
#define kNtSrcerase    0x00440328u /* src & ~dst */
#define kNtNotsrccopy  0x00330008u /* ~src */
#define kNtNotsrcerase 0x001100A6u /* ~src & ~dst */
#define kNtMergecopy   0x00C000CAu /* (src & pattern) */
#define kNtMergepaint  0x00BB0226u /* ~src | dst */
#define kNtPatcopy     0x00F00021u /* pat */
#define kNtPatpaint    0x00FB0A09u /* wut */
#define kNtPatinvert   0x005A0049u /* pat ^ dst */
#define kNtDstinvert   0x00550009u /* ~dst */
#define kNtBlackness   0x00000042u /* black */
#define kNtWhiteness   0x00FF0062u /* white */

#endif /* COSMOPOLITAN_LIBC_NT_ENUM_BITBLT_H_ */
