#ifndef COSMOPOLITAN_LIBC_NT_ENUM_MESSAGEBOXTYPE_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_MESSAGEBOXTYPE_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

enum MessageBoxType {
  kNtMbOk = 0x00000000,
  kNtMbOkcancel = 0x00000001,
  kNtMbAbortretryignore = 0x00000002,
  kNtMbYesnocancel = 0x00000003,
  kNtMbYesno = 0x00000004,
  kNtMbRetrycancel = 0x00000005,
  kNtMbCanceltrycontinue = 0x00000006,
  kNtMbIconhand = 0x00000010,
  kNtMbIconquestion = 0x00000020,
  kNtMbIconexclamation = 0x00000030,
  kNtMbIconasterisk = 0x00000040,
  kNtMbUsericon = 0x00000080,
  kNtMbIconwarning = kNtMbIconexclamation,
  kNtMbIconerror = kNtMbIconhand,
  kNtMbIconinformation = kNtMbIconasterisk,
  kNtMbIconstop = kNtMbIconhand,
  kNtMbDefbutton1 = 0x00000000,
  kNtMbDefbutton2 = 0x00000100,
  kNtMbDefbutton3 = 0x00000200,
  kNtMbDefbutton4 = 0x00000300,
  kNtMbApplmodal = 0x00000000,
  kNtMbSystemmodal = 0x00001000,
  kNtMbTaskmodal = 0x00002000,
  kNtMbHelp = 0x00004000,
  kNtMbNofocus = 0x00008000,
  kNtMbSetforeground = 0x00010000,
  kNtMbDefaultDesktopOnly = 0x00020000,
  kNtMbTopmost = 0x00040000,
  kNtMbRight = 0x00080000,
  kNtMbRtlreading = 0x00100000,
  kNtMbServiceNotification = 0x00200000,
  kNtMbServiceNotificationNt3x = 0x00040000,
  kNtMbTypemask = 0x0000000F,
  kNtMbIconmask = 0x000000F0,
  kNtMbDefmask = 0x00000F00,
  kNtMbModemask = 0x00003000,
  kNtMbMiscmask = 0x0000C000
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_ENUM_MESSAGEBOXTYPE_H_ */
