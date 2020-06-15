#define GetEnvironmentVariable(...) __imp_GetEnvironmentVariableW(__VA_ARGS__)
#define GetPriorityClass(...)       __imp_GetPriorityClass(__VA_ARGS__)
#define SetPriorityClass(...)       __imp_SetPriorityClass(__VA_ARGS__)

extern typeof(GetEnvironmentVariable) *const
    __imp_GetEnvironmentVariableW __msabi;
extern typeof(GetPriorityClass) *const __imp_GetPriorityClass __msabi;
extern typeof(SetPriorityClass) *const __imp_SetPriorityClass __msabi;
