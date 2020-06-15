#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_FILEMAILSLOTQUERYINFORMATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_FILEMAILSLOTQUERYINFORMATION_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtFileMailslotQueryInformation {
  uint32_t MaximumMessageSize;
  uint32_t MailslotQuota;
  uint32_t NextMessageSize;
  uint32_t MessagesAvailable;
  int64_t ReadTimeout;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_FILEMAILSLOTQUERYINFORMATION_H_ */
