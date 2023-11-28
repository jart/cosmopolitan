#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_CLIENTID_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_CLIENTID_H_

struct NtClientId {
  void *UniqueProcess;
  void *UniqueThread;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_CLIENTID_H_ */
