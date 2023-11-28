#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_FILTER_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_FILTER_H_
COSMOPOLITAN_C_START_

#define BPF_MAJOR_VERSION 1
#define BPF_MINOR_VERSION 1

struct sock_filter {
  uint16_t code;
  uint8_t jt;
  uint8_t jf;
  uint32_t k;
};

struct sock_fprog {
  unsigned short len;
  const struct sock_filter *filter;
};

#define BPF_RVAL(code)   ((code)&0x18)
#define BPF_A            0x10
#define BPF_MISCOP(code) ((code)&0xf8)
#define BPF_TAX          0x00
#define BPF_TXA          0x80

#define BPF_STMT(code, k) \
  { (unsigned short)(code), 0, 0, k }
#define BPF_JUMP(code, k, jumptrue, jumpfalse) \
  { (unsigned short)(code), jumptrue, jumpfalse, k }

#define BPF_MEMWORDS 16

#define SKF_AD_OFF              (-0x1000)
#define SKF_AD_PROTOCOL         0
#define SKF_AD_PKTTYPE          4
#define SKF_AD_IFINDEX          8
#define SKF_AD_NLATTR           12
#define SKF_AD_NLATTR_NEST      16
#define SKF_AD_MARK             20
#define SKF_AD_QUEUE            24
#define SKF_AD_HATYPE           28
#define SKF_AD_RXHASH           32
#define SKF_AD_CPU              36
#define SKF_AD_ALU_XOR_X        40
#define SKF_AD_VLAN_TAG         44
#define SKF_AD_VLAN_TAG_PRESENT 48
#define SKF_AD_PAY_OFFSET       52
#define SKF_AD_RANDOM           56
#define SKF_AD_VLAN_TPID        60
#define SKF_AD_MAX              64
#define SKF_NET_OFF             (-0x100000)
#define SKF_LL_OFF              (-0x200000)
#define BPF_NET_OFF             SKF_NET_OFF
#define BPF_LL_OFF              SKF_LL_OFF

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_FILTER_H_ */
