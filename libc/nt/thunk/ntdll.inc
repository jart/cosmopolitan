#define NtYieldExecution(...) __imp_NtYieldExecution(__VA_ARGS__)

extern typeof(NtYieldExecution) *const __imp_NtYieldExecution __msabi;
