#define NtDelayExecution(...) __imp_NtDelayExecution(__VA_ARGS__)

extern typeof(NtDelayExecution) *const __imp_NtDelayExecution __msabi;
