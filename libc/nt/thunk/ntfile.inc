#define NtQueryInformationFile(...) __imp_NtQueryInformationFile(__VA_ARGS__)

extern typeof(NtQueryInformationFile) *const
    __imp_NtQueryInformationFile __msabi;
