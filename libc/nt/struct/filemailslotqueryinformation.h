#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_FILEMAILSLOTQUERYINFORMATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_FILEMAILSLOTQUERYINFORMATION_H_

struct NtFileMailslotQueryInformation {
  uint32_t MaximumMessageSize;
  uint32_t MailslotQuota;
  uint32_t NextMessageSize;
  uint32_t MessagesAvailable;
  int64_t ReadTimeout;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_FILEMAILSLOTQUERYINFORMATION_H_ */
