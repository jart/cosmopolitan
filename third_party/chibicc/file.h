#ifndef COSMOPOLITAN_THIRD_PARTY_CHIBICC_FILE_H_
#define COSMOPOLITAN_THIRD_PARTY_CHIBICC_FILE_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

char *skip_bom(char *);
char *read_file(const char *);
void remove_backslash_newline(char *);
void canonicalize_newline(char *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_CHIBICC_FILE_H_ */
