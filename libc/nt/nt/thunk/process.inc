#define NtQueryInformationProcess(...) \
  __imp_NtQueryInformationProcess(__VA_ARGS__)

extern typeof(NtQueryInformationProcess) *const
    __imp_NtQueryInformationProcess __msabi;
