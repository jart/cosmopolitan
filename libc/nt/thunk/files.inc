#define FlushFileBuffers(...) __imp_FlushFileBuffers(__VA_ARGS__)

extern typeof(FlushFileBuffers) *const __imp_FlushFileBuffers __msabi;
