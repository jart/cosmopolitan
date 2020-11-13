#ifndef COSMOPOLITAN_LIBC_NT_ENUM_WS_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_WS_H_

#define kNtWsOverlapped   0x00000000
#define kNtWsPopup        0x80000000
#define kNtWsChild        0x40000000
#define kNtWsMinimize     0x20000000
#define kNtWsVisible      0x10000000
#define kNtWsDisabled     0x08000000
#define kNtWsClipsiblings 0x04000000
#define kNtWsClipchildren 0x02000000
#define kNtWsMaximize     0x01000000
#define kNtWsCaption      0x00C00000
#define kNtWsBorder       0x00800000
#define kNtWsDlgframe     0x00400000
#define kNtWsVscroll      0x00200000
#define kNtWsHscroll      0x00100000
#define kNtWsSysmenu      0x00080000
#define kNtWsThickframe   0x00040000
#define kNtWsGroup        0x00020000
#define kNtWsTabstop      0x00010000
#define kNtWsMinimizebox  0x00020000
#define kNtWsMaximizebox  0x00010000
#define kNtWsTiled        kNtWsOverlapped
#define kNtWsIconic       kNtWsMinimize
#define kNtWsSizebox      kNtWsThickframe
#define kNtWsTiledwindow  kNtWsOverlappedwindow
#define kNtWsOverlappedwindow                                        \
  (kNtWsOverlapped | kNtWsCaption | kNtWsSysmenu | kNtWsThickframe | \
   kNtWsMinimizebox | kNtWsMaximizebox)
#define kNtWsPopupwindow (kNtWsPopup | kNtWsBorder | kNtWsSysmenu)

#define kNtWsExDlgmodalframe       0x00000001
#define kNtWsExNoparentnotify      0x00000004
#define kNtWsExTopmost             0x00000008
#define kNtWsExAcceptfiles         0x00000010
#define kNtWsExTransparent         0x00000020
#define kNtWsExMdichild            0x00000040
#define kNtWsExToolwindow          0x00000080
#define kNtWsExWindowedge          0x00000100
#define kNtWsExClientedge          0x00000200
#define kNtWsExContexthelp         0x00000400
#define kNtWsExRight               0x00001000
#define kNtWsExLeft                0x00000000
#define kNtWsExRtlreading          0x00002000
#define kNtWsExLtrreading          0x00000000
#define kNtWsExLeftscrollbar       0x00004000
#define kNtWsExRightscrollbar      0x00000000
#define kNtWsExControlparent       0x00010000
#define kNtWsExStaticedge          0x00020000
#define kNtWsExAppwindow           0x00040000
#define kNtWsExNoinheritlayout     0x00100000
#define kNtWsExNoredirectionbitmap 0x00200000
#define kNtWsExLayoutrtl           0x00400000
#define kNtWsExComposited          0x02000000
#define kNtWsExNoactivate          0x08000000

#define kNtWsExOverlappedwindow (kNtWsExWindowedge | kNtWsExClientedge)
#define kNtWsExPalettewindow \
  (kNtWsExWindowedge | kNtWsExToolwindow | kNtWsExTopmost)

#endif /* COSMOPOLITAN_LIBC_NT_ENUM_WS_H_ */
