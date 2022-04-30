#define GetProcessTimes(...) __imp_GetProcessTimes(__VA_ARGS__)
extern typeof(GetProcessTimes) *const __imp_GetProcessTimes __msabi;

#define GetThreadTimes(...) __imp_GetThreadTimes(__VA_ARGS__)
extern typeof(GetThreadTimes) *const __imp_GetThreadTimes __msabi;

#define GetUserName(...) __imp_GetUserNameW(__VA_ARGS__)
extern typeof(GetUserName) *const __imp_GetUserNameW __msabi;
