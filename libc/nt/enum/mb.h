#ifndef COSMOPOLITAN_LIBC_NT_ENUM_MB_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_MB_H_

#define kNtMbOk                      0x00000000
#define kNtMbOkcancel                0x00000001
#define kNtMbAbortretryignore        0x00000002
#define kNtMbYesnocancel             0x00000003
#define kNtMbYesno                   0x00000004
#define kNtMbRetrycancel             0x00000005
#define kNtMbCanceltrycontinue       0x00000006
#define kNtMbIconhand                0x00000010
#define kNtMbIconquestion            0x00000020
#define kNtMbIconexclamation         0x00000030
#define kNtMbIconasterisk            0x00000040
#define kNtMbUsericon                0x00000080
#define kNtMbIconwarning             kNtMbIconexclamation
#define kNtMbIconerror               kNtMbIconhand
#define kNtMbIconinformation         kNtMbIconasterisk
#define kNtMbIconstop                kNtMbIconhand
#define kNtMbDefbutton1              0x00000000
#define kNtMbDefbutton2              0x00000100
#define kNtMbDefbutton3              0x00000200
#define kNtMbDefbutton4              0x00000300
#define kNtMbApplmodal               0x00000000
#define kNtMbSystemmodal             0x00001000
#define kNtMbTaskmodal               0x00002000
#define kNtMbHelp                    0x00004000
#define kNtMbNofocus                 0x00008000
#define kNtMbSetforeground           0x00010000
#define kNtMbDefaultDesktopOnly      0x00020000
#define kNtMbTopmost                 0x00040000
#define kNtMbRight                   0x00080000
#define kNtMbRtlreading              0x00100000
#define kNtMbServiceNotification     0x00200000
#define kNtMbServiceNotificationNt3x 0x00040000
#define kNtMbTypemask                0x0000000F
#define kNtMbIconmask                0x000000F0
#define kNtMbDefmask                 0x00000F00
#define kNtMbModemask                0x00003000
#define kNtMbMiscmask                0x0000C000

#endif /* COSMOPOLITAN_LIBC_NT_ENUM_MB_H_ */
