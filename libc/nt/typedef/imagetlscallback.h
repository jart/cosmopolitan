#ifndef COSMOPOLITAN_LIBC_NT_TYPEDEF_IMAGETLSCALLBACK_H_
#define COSMOPOLITAN_LIBC_NT_TYPEDEF_IMAGETLSCALLBACK_H_

typedef void (*NtImageTlsCallback)(void *DllHandle, uint32_t Reason,
                                   void *Reserved);

#endif /* COSMOPOLITAN_LIBC_NT_TYPEDEF_IMAGETLSCALLBACK_H_ */
