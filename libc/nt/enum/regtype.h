#ifndef COSMOPOLITAN_LIBC_NT_ENUM_REGTYPE_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_REGTYPE_H_

#define kNtRegNone                     0
#define kNtRegSz                       1 /* UTF-16 string */
#define kNtRegExpandSz                 2 /* UTF-16 string w/ env vars refs */
#define kNtRegBinary                   3
#define kNtRegDword                    4
#define kNtRegDwordBigEndian           5
#define kNtRegLink                     6
#define kNtRegMultiSz                  7 /* UTF-16 double-nul-terminated */
#define kNtRegResourceList             8
#define kNtRegFullResourceDescriptor   9
#define kNtRegResourceRequirementsList 10
#define kNtRegQword                    11

#endif /* COSMOPOLITAN_LIBC_NT_ENUM_REGTYPE_H_ */
