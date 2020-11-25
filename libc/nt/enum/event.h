#ifndef COSMOPOLITAN_LIBC_NT_ENUM_EVENT_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_EVENT_H_

#define kNtEventSystemSound            0x0001
#define kNtEventSystemAlert            0x0002
#define kNtEventSystemForeground       0x0003
#define kNtEventSystemMenustart        0x0004
#define kNtEventSystemMenuend          0x0005
#define kNtEventSystemMenupopupstart   0x0006
#define kNtEventSystemMenupopupend     0x0007
#define kNtEventSystemCapturestart     0x0008
#define kNtEventSystemCaptureend       0x0009
#define kNtEventSystemMovesizestart    0x000A
#define kNtEventSystemMovesizeend      0x000B
#define kNtEventSystemContexthelpstart 0x000C
#define kNtEventSystemContexthelpend   0x000D
#define kNtEventSystemDragdropstart    0x000E
#define kNtEventSystemDragdropend      0x000F
#define kNtEventSystemDialogstart      0x0010
#define kNtEventSystemDialogend        0x0011
#define kNtEventSystemScrollingstart   0x0012
#define kNtEventSystemScrollingend     0x0013
#define kNtEventSystemSwitchstart      0x0014
#define kNtEventSystemSwitchend        0x0015
#define kNtEventSystemMinimizestart    0x0016
#define kNtEventSystemMinimizeend      0x0017

#define kNtEventConsoleCaret            0x4001
#define kNtEventConsoleUpdateRegion     0x4002
#define kNtEventConsoleUpdateSimple     0x4003
#define kNtEventConsoleUpdateScroll     0x4004
#define kNtEventConsoleLayout           0x4005
#define kNtEventConsoleStartApplication 0x4006
#define kNtEventConsoleEndApplication   0x4007

#define kNtEventObjectCreate            0x8000
#define kNtEventObjectDestroy           0x8001
#define kNtEventObjectShow              0x8002
#define kNtEventObjectHide              0x8003
#define kNtEventObjectReorder           0x8004
#define kNtEventObjectFocus             0x8005
#define kNtEventObjectSelection         0x8006
#define kNtEventObjectSelectionadd      0x8007
#define kNtEventObjectSelectionremove   0x8008
#define kNtEventObjectSelectionwithin   0x8009
#define kNtEventObjectStatechange       0x800A
#define kNtEventObjectLocationchange    0x800B
#define kNtEventObjectNamechange        0x800C
#define kNtEventObjectDescriptionchange 0x800D
#define kNtEventObjectValuechange       0x800E
#define kNtEventObjectParentchange      0x800F
#define kNtEventObjectHelpchange        0x8010
#define kNtEventObjectDefactionchange   0x8011
#define kNtEventObjectAcceleratorchange 0x8012

#endif /* COSMOPOLITAN_LIBC_NT_ENUM_EVENT_H_ */
