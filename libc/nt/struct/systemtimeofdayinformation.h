#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_SYSTEMTIMEOFDAYINFORMATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_SYSTEMTIMEOFDAYINFORMATION_H_

struct NtSystemTimeofdayInformation {
  int64_t BootTime;
  int64_t CurrentTime;
  int64_t TimeZoneBias;
  uint32_t CurrentTimeZoneId;
  unsigned char Reserved1[20];
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_SYSTEMTIMEOFDAYINFORMATION_H_ */
