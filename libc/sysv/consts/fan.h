#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_FAN_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_FAN_H_

#define FAN_CLASS_NOTIF              0
#define FAN_ACCESS                   1
#define FAN_ACCESS_PERM              0x020000
#define FAN_ALLOW                    1
#define FAN_ALL_CLASS_BITS           12
#define FAN_ALL_EVENTS               59
#define FAN_ALL_INIT_FLAGS           63
#define FAN_ALL_MARK_FLAGS           255
#define FAN_ALL_OUTGOING_EVENTS      0x03403b
#define FAN_ALL_PERM_EVENTS          0x030000
#define FAN_CLASS_CONTENT            4
#define FAN_CLASS_PRE_CONTENT        8
#define FAN_CLOEXEC                  1
#define FAN_CLOSE                    24
#define FAN_CLOSE_NOWRITE            0x10
#define FAN_CLOSE_WRITE              8
#define FAN_DENY                     2
#define FAN_EVENT_METADATA_LEN       24
#define FAN_EVENT_ON_CHILD           0x08000000
#define FAN_MARK_ADD                 1
#define FAN_MARK_DONT_FOLLOW         4
#define FAN_MARK_FLUSH               0x80
#define FAN_MARK_IGNORED_MASK        0x20
#define FAN_MARK_IGNORED_SURV_MODIFY 0x40
#define FAN_MARK_MOUNT               0x10
#define FAN_MARK_ONLYDIR             8
#define FAN_MARK_REMOVE              2
#define FAN_MODIFY                   2
#define FAN_NOFD                     -1
#define FAN_NONBLOCK                 2
#define FAN_ONDIR                    0x40000000
#define FAN_OPEN                     0x20
#define FAN_OPEN_PERM                0x010000
#define FAN_Q_OVERFLOW               0x4000
#define FAN_UNLIMITED_MARKS          0x20
#define FAN_UNLIMITED_QUEUE          0x10

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_FAN_H_ */
