#ifndef COSMOPOLITAN_LIBC_NT_TYPEDEF_IMAGETLSCALLBACK_H_
#define COSMOPOLITAN_LIBC_NT_TYPEDEF_IMAGETLSCALLBACK_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

typedef void (*NtImageTlsCallback)(void *DllHandle, uint32_t Reason,
                                   void *Reserved);

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_TYPEDEF_IMAGETLSCALLBACK_H_ */
