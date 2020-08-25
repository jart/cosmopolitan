#define CopyFile(...)         __imp_CopyFileW(__VA_ARGS__)
#define FlushFileBuffers(...) __imp_FlushFileBuffers(__VA_ARGS__)

extern typeof(CopyFile) *const __imp_CopyFileW __msabi;
extern typeof(FlushFileBuffers) *const __imp_FlushFileBuffers __msabi;
