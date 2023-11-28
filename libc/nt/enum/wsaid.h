#ifndef COSMOPOLITAN_LIBC_NT_ENUM_WSAID_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_WSAID_H_
COSMOPOLITAN_C_START_

#define WSAID_WSAPOLL                                \
  {                                                  \
    0x18C76F85, 0xDC66, 0x4964, {                    \
      0x97, 0x2E, 0x23, 0xC2, 0x72, 0x38, 0x31, 0x2B \
    }                                                \
  }

#define WSAID_WSARECVMSG                             \
  {                                                  \
    0xf689d7c8, 0x6f1f, 0x436b, {                    \
      0x8a, 0x53, 0xe5, 0x4f, 0xe3, 0x51, 0xc3, 0x22 \
    }                                                \
  }

#define WSAID_WSASENDMSG                             \
  {                                                  \
    0xa441e712, 0x754f, 0x43ca, {                    \
      0x84, 0xa7, 0x0d, 0xee, 0x44, 0xcf, 0x60, 0x6d \
    }                                                \
  }

#define WSAID_CONNECTEX                              \
  {                                                  \
    0x25a207b9, 0xddf3, 0x4660, {                    \
      0x8e, 0xe9, 0x76, 0xe5, 0x8c, 0x74, 0x06, 0x3e \
    }                                                \
  }

#define WSAID_ACCEPTEX                               \
  {                                                  \
    0xb5367df1, 0xcbac, 0x11cf, {                    \
      0x95, 0xca, 0x00, 0x80, 0x5f, 0x48, 0xa1, 0x92 \
    }                                                \
  }

#define WSAID_GETACCEPTEXSOCKADDRS                   \
  {                                                  \
    0xb5367df2, 0xcbac, 0x11cf, {                    \
      0x95, 0xca, 0x00, 0x80, 0x5f, 0x48, 0xa1, 0x92 \
    }                                                \
  }

#define WSAID_TRANSMITFILE                           \
  {                                                  \
    0xb5367df0, 0xcbac, 0x11cf, {                    \
      0x95, 0xca, 0x00, 0x80, 0x5f, 0x48, 0xa1, 0x92 \
    }                                                \
  }

#define WSAID_TRANSMITPACKETS                        \
  {                                                  \
    0xd9689da0, 0x1f90, 0x11d3, {                    \
      0x99, 0x71, 0x00, 0xc0, 0x4f, 0x68, 0xc8, 0x76 \
    }                                                \
  }

#define WSAID_DISCONNECTEX                           \
  {                                                  \
    0x7fda2e11, 0x8630, 0x436f, {                    \
      0xa0, 0x31, 0xf5, 0x36, 0xa6, 0xee, 0xc1, 0x57 \
    }                                                \
  }

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_ENUM_WSAID_H_ */
