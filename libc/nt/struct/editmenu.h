#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_EDITMENU_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_EDITMENU_H_

struct NtEditMenu {
  int64_t hmenu;
  uint16_t idEdit;
  uint16_t idCut;
  uint16_t idCopy;
  uint16_t idPaste;
  uint16_t idClear;
  uint16_t idUndo;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_EDITMENU_H_ */
