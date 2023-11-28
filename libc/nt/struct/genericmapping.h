#ifndef COSMOPOLITAN_LIBC_NT_GENERICMAPPING_H_
#define COSMOPOLITAN_LIBC_NT_GENERICMAPPING_H_

struct NtGenericMapping {
  unsigned int GenericRead;
  unsigned int GenericWrite;
  unsigned int GenericExecute;
  unsigned int GenericAll;
};

#endif /* COSMOPOLITAN_LIBC_NT_GENERICMAPPING_H_ */
