#ifndef COSMOPOLITAN_LIBC_NT_ENUM_E_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_E_H_

#define SUCCEEDED(hr) ((hr) >= 0)
#define FAILED(hr)    ((hr) < 0)

#define S_OK    0
#define S_FALSE 1

#define E_UNEXPECTED           ((int)0x8000FFFF)
#define E_NOTIMPL              ((int)0x80004001)
#define E_OUTOFMEMORY          ((int)0x8007000E)
#define E_INVALIDARG           ((int)0x80070057)
#define E_NOINTERFACE          ((int)0x80004002)
#define E_POINTER              ((int)0x80004003)
#define E_HANDLE               ((int)0x80070006)
#define E_ABORT                ((int)0x80004004)
#define E_FAIL                 ((int)0x80004005)
#define E_ACCESSDENIED         ((int)0x80070005)
#define E_PENDING              ((int)0x8000000A)
#define E_BOUNDS               ((int)0x8000000B)
#define E_CHANGED_STATE        ((int)0x8000000C)
#define E_ILLEGAL_STATE_CHANGE ((int)0x8000000D)
#define E_ILLEGAL_METHOD_CALL  ((int)0x8000000E)

#endif /* COSMOPOLITAN_LIBC_NT_ENUM_E_H_ */
