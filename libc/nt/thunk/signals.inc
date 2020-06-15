#define SetErrorMode(...) __imp_SetErrorMode(__VA_ARGS__)
#define AddVectoredExceptionHandler(...) \
  __imp_AddVectoredExceptionHandler(__VA_ARGS__)
#define AddVectoredContinueHandler(...) \
  __imp_AddVectoredContinueHandler(__VA_ARGS__)
#define RemoveVectoredExceptionHandler(...) \
  __imp_RemoveVectoredExceptionHandler(__VA_ARGS__)
#define RemoveVectoredContinueHandler(...) \
  __imp_RemoveVectoredContinueHandler(__VA_ARGS__)
#define SetUnhandledExceptionFilter(...) \
  __imp_SetUnhandledExceptionFilter(__VA_ARGS__)

extern typeof(SetErrorMode) *const __imp_SetErrorMode __msabi;
extern typeof(AddVectoredExceptionHandler) *const
    __imp_AddVectoredExceptionHandler __msabi;
extern typeof(AddVectoredContinueHandler) *const
    __imp_AddVectoredContinueHandler __msabi;
extern typeof(RemoveVectoredExceptionHandler) *const
    __imp_RemoveVectoredExceptionHandler __msabi;
extern typeof(RemoveVectoredContinueHandler) *const
    __imp_RemoveVectoredContinueHandler __msabi;
extern typeof(SetUnhandledExceptionFilter) *const
    __imp_SetUnhandledExceptionFilter __msabi;
