#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_BPF_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_BPF_H_
COSMOPOLITAN_C_START_

#define BPF_MAXINSNS 4096

#define BPF_CLASS(code) ((code)&0x07)
#define BPF_LD          0x00 /* load into accumulator */
#define BPF_LDX         0x01 /* load into index register */
#define BPF_ST          0x02 /* store from immediate */
#define BPF_STX         0x03 /* store from register */
#define BPF_ALU         0x04 /* 32-bit arithmetic */
#define BPF_JMP         0x05
#define BPF_RET         0x06
#define BPF_MISC        0x07

#define BPF_SIZE(code) ((code)&0x18)
#define BPF_W          0x00 /* 32-bit */
#define BPF_H          0x08 /* 16-bit */
#define BPF_B          0x10 /*  8-bit */
#define BPF_DW         0x18 /* 64-bit (eBPF only) */

#define BPF_MODE(code) ((code)&0xe0)
#define BPF_IMM        0x00 /* 64-bit immediate */
#define BPF_ABS        0x20
#define BPF_IND        0x40
#define BPF_MEM        0x60
#define BPF_LEN        0x80
#define BPF_MSH        0xa0

#define BPF_OP(code) ((code)&0xf0)
#define BPF_ADD      0x00
#define BPF_SUB      0x10
#define BPF_MUL      0x20
#define BPF_DIV      0x30
#define BPF_OR       0x40
#define BPF_AND      0x50
#define BPF_LSH      0x60
#define BPF_RSH      0x70
#define BPF_NEG      0x80
#define BPF_MOD      0x90
#define BPF_XOR      0xa0

#define BPF_SRC(code) ((code)&0x08)
#define BPF_JA        0x00
#define BPF_JEQ       0x10
#define BPF_JGT       0x20
#define BPF_JGE       0x30
#define BPF_JSET      0x40
#define BPF_K         0x00
#define BPF_X         0x08

#define BPF_JMP32   0x06
#define BPF_ALU64   0x07
#define BPF_ATOMIC  0xc0
#define BPF_XADD    0xc0
#define BPF_MOV     0xb0
#define BPF_ARSH    0xc0
#define BPF_END     0xd0
#define BPF_TO_LE   0x00
#define BPF_TO_BE   0x08
#define BPF_FROM_LE BPF_TO_LE
#define BPF_FROM_BE BPF_TO_BE
#define BPF_JNE     0x50 /* != */
#define BPF_JLT     0xa0 /* unsigned < */
#define BPF_JLE     0xb0 /* unsigned <= */
#define BPF_JSGT    0x60 /* signed > */
#define BPF_JSGE    0x70 /* signed >= */
#define BPF_JSLT    0xc0 /* signed < */
#define BPF_JSLE    0xd0 /* signed <= */
#define BPF_CALL    0x80 /* call */
#define BPF_EXIT    0x90 /* ret */
#define BPF_FETCH   0x01
#define BPF_XCHG    (0xe0 | BPF_FETCH)
#define BPF_CMPXCHG (0xf0 | BPF_FETCH)
#define MAX_BPF_REG __MAX_BPF_REG

#define BPF_REG_0     0
#define BPF_REG_1     1
#define BPF_REG_2     2
#define BPF_REG_3     3
#define BPF_REG_4     4
#define BPF_REG_5     5
#define BPF_REG_6     6
#define BPF_REG_7     7
#define BPF_REG_8     8
#define BPF_REG_9     9
#define BPF_REG_10    10
#define __MAX_BPF_REG 11

#define BPF_MAP_CREATE                  0
#define BPF_MAP_LOOKUP_ELEM             1
#define BPF_MAP_UPDATE_ELEM             2
#define BPF_MAP_DELETE_ELEM             3
#define BPF_MAP_GET_NEXT_KEY            4
#define BPF_PROG_LOAD                   5
#define BPF_OBJ_PIN                     6
#define BPF_OBJ_GET                     7
#define BPF_PROG_ATTACH                 8
#define BPF_PROG_DETACH                 9
#define BPF_PROG_TEST_RUN               10
#define BPF_PROG_GET_NEXT_ID            11
#define BPF_MAP_GET_NEXT_ID             12
#define BPF_PROG_GET_FD_BY_ID           13
#define BPF_MAP_GET_FD_BY_ID            14
#define BPF_OBJ_GET_INFO_BY_FD          15
#define BPF_PROG_QUERY                  16
#define BPF_RAW_TRACEPOINT_OPEN         17
#define BPF_BTF_LOAD                    18
#define BPF_BTF_GET_FD_BY_ID            19
#define BPF_TASK_FD_QUERY               20
#define BPF_MAP_LOOKUP_AND_DELETE_ELEM  21
#define BPF_MAP_FREEZE                  22
#define BPF_BTF_GET_NEXT_ID             23
#define BPF_MAP_LOOKUP_BATCH            24
#define BPF_MAP_LOOKUP_AND_DELETE_BATCH 25
#define BPF_MAP_UPDATE_BATCH            26
#define BPF_MAP_DELETE_BATCH            27
#define BPF_LINK_CREATE                 28
#define BPF_LINK_UPDATE                 29
#define BPF_LINK_GET_FD_BY_ID           30
#define BPF_LINK_GET_NEXT_ID            31
#define BPF_ENABLE_STATS                32
#define BPF_ITER_CREATE                 33
#define BPF_LINK_DETACH                 34
#define BPF_PROG_BIND_MAP               35
#define BPF_PROG_RUN                    BPF_PROG_TEST_RUN

#define BPF_MAP_TYPE_UNSPEC                0
#define BPF_MAP_TYPE_HASH                  1
#define BPF_MAP_TYPE_ARRAY                 2
#define BPF_MAP_TYPE_PROG_ARRAY            3
#define BPF_MAP_TYPE_PERF_EVENT_ARRAY      4
#define BPF_MAP_TYPE_PERCPU_HASH           5
#define BPF_MAP_TYPE_PERCPU_ARRAY          6
#define BPF_MAP_TYPE_STACK_TRACE           7
#define BPF_MAP_TYPE_CGROUP_ARRAY          8
#define BPF_MAP_TYPE_LRU_HASH              9
#define BPF_MAP_TYPE_LRU_PERCPU_HASH       10
#define BPF_MAP_TYPE_LPM_TRIE              11
#define BPF_MAP_TYPE_ARRAY_OF_MAPS         12
#define BPF_MAP_TYPE_HASH_OF_MAPS          13
#define BPF_MAP_TYPE_DEVMAP                14
#define BPF_MAP_TYPE_SOCKMAP               15
#define BPF_MAP_TYPE_CPUMAP                16
#define BPF_MAP_TYPE_XSKMAP                17
#define BPF_MAP_TYPE_SOCKHASH              18
#define BPF_MAP_TYPE_CGROUP_STORAGE        19
#define BPF_MAP_TYPE_REUSEPORT_SOCKARRAY   20
#define BPF_MAP_TYPE_PERCPU_CGROUP_STORAGE 21
#define BPF_MAP_TYPE_QUEUE                 22
#define BPF_MAP_TYPE_STACK                 23
#define BPF_MAP_TYPE_SK_STORAGE            24
#define BPF_MAP_TYPE_DEVMAP_HASH           25
#define BPF_MAP_TYPE_STRUCT_OPS            26
#define BPF_MAP_TYPE_RINGBUF               27
#define BPF_MAP_TYPE_INODE_STORAGE         28
#define BPF_MAP_TYPE_TASK_STORAGE          29

#define BPF_PROG_TYPE_UNSPEC                  0
#define BPF_PROG_TYPE_SOCKET_FILTER           1
#define BPF_PROG_TYPE_KPROBE                  2
#define BPF_PROG_TYPE_SCHED_CLS               3
#define BPF_PROG_TYPE_SCHED_ACT               4
#define BPF_PROG_TYPE_TRACEPOINT              5
#define BPF_PROG_TYPE_XDP                     6
#define BPF_PROG_TYPE_PERF_EVENT              7
#define BPF_PROG_TYPE_CGROUP_SKB              8
#define BPF_PROG_TYPE_CGROUP_SOCK             9
#define BPF_PROG_TYPE_LWT_IN                  10
#define BPF_PROG_TYPE_LWT_OUT                 11
#define BPF_PROG_TYPE_LWT_XMIT                12
#define BPF_PROG_TYPE_SOCK_OPS                13
#define BPF_PROG_TYPE_SK_SKB                  14
#define BPF_PROG_TYPE_CGROUP_DEVICE           15
#define BPF_PROG_TYPE_SK_MSG                  16
#define BPF_PROG_TYPE_RAW_TRACEPOINT          17
#define BPF_PROG_TYPE_CGROUP_SOCK_ADDR        18
#define BPF_PROG_TYPE_LWT_SEG6LOCAL           19
#define BPF_PROG_TYPE_LIRC_MODE2              20
#define BPF_PROG_TYPE_SK_REUSEPORT            21
#define BPF_PROG_TYPE_FLOW_DISSECTOR          22
#define BPF_PROG_TYPE_CGROUP_SYSCTL           23
#define BPF_PROG_TYPE_RAW_TRACEPOINT_WRITABLE 24
#define BPF_PROG_TYPE_CGROUP_SOCKOPT          25
#define BPF_PROG_TYPE_TRACING                 26
#define BPF_PROG_TYPE_STRUCT_OPS              27
#define BPF_PROG_TYPE_EXT                     28
#define BPF_PROG_TYPE_LSM                     29
#define BPF_PROG_TYPE_SK_LOOKUP               30
#define BPF_PROG_TYPE_SYSCALL                 31

#define BPF_CGROUP_INET_INGRESS            0
#define BPF_CGROUP_INET_EGRESS             1
#define BPF_CGROUP_INET_SOCK_CREATE        2
#define BPF_CGROUP_SOCK_OPS                3
#define BPF_SK_SKB_STREAM_PARSER           4
#define BPF_SK_SKB_STREAM_VERDICT          5
#define BPF_CGROUP_DEVICE                  6
#define BPF_SK_MSG_VERDICT                 7
#define BPF_CGROUP_INET4_BIND              8
#define BPF_CGROUP_INET6_BIND              9
#define BPF_CGROUP_INET4_CONNECT           10
#define BPF_CGROUP_INET6_CONNECT           11
#define BPF_CGROUP_INET4_POST_BIND         12
#define BPF_CGROUP_INET6_POST_BIND         13
#define BPF_CGROUP_UDP4_SENDMSG            14
#define BPF_CGROUP_UDP6_SENDMSG            15
#define BPF_LIRC_MODE2                     16
#define BPF_FLOW_DISSECTOR                 17
#define BPF_CGROUP_SYSCTL                  18
#define BPF_CGROUP_UDP4_RECVMSG            19
#define BPF_CGROUP_UDP6_RECVMSG            20
#define BPF_CGROUP_GETSOCKOPT              21
#define BPF_CGROUP_SETSOCKOPT              22
#define BPF_TRACE_RAW_TP                   23
#define BPF_TRACE_FENTRY                   24
#define BPF_TRACE_FEXIT                    25
#define BPF_MODIFY_RETURN                  26
#define BPF_LSM_MAC                        27
#define BPF_TRACE_ITER                     28
#define BPF_CGROUP_INET4_GETPEERNAME       29
#define BPF_CGROUP_INET6_GETPEERNAME       30
#define BPF_CGROUP_INET4_GETSOCKNAME       31
#define BPF_CGROUP_INET6_GETSOCKNAME       32
#define BPF_XDP_DEVMAP                     33
#define BPF_CGROUP_INET_SOCK_RELEASE       34
#define BPF_XDP_CPUMAP                     35
#define BPF_SK_LOOKUP                      36
#define BPF_XDP                            37
#define BPF_SK_SKB_VERDICT                 38
#define BPF_SK_REUSEPORT_SELECT            39
#define BPF_SK_REUSEPORT_SELECT_OR_MIGRATE 40
#define __MAX_BPF_ATTACH_TYPE              41

#define MAX_BPF_ATTACH_TYPE __MAX_BPF_ATTACH_TYPE

#define BPF_LINK_TYPE_UNSPEC         0
#define BPF_LINK_TYPE_RAW_TRACEPOINT 1
#define BPF_LINK_TYPE_TRACING        2
#define BPF_LINK_TYPE_CGROUP         3
#define BPF_LINK_TYPE_ITER           4
#define BPF_LINK_TYPE_NETNS          5
#define BPF_LINK_TYPE_XDP            6
#define MAX_BPF_LINK_TYPE            7

#define BPF_F_ALLOW_OVERRIDE     (1U << 0)
#define BPF_F_ALLOW_MULTI        (1U << 1)
#define BPF_F_REPLACE            (1U << 2)
#define BPF_F_STRICT_ALIGNMENT   (1U << 0)
#define BPF_F_ANY_ALIGNMENT      (1U << 1)
#define BPF_F_TEST_RND_HI32      (1U << 2)
#define BPF_F_TEST_STATE_FREQ    (1U << 3)
#define BPF_F_SLEEPABLE          (1U << 4)
#define BPF_PSEUDO_MAP_FD        1
#define BPF_PSEUDO_MAP_IDX       5
#define BPF_PSEUDO_MAP_VALUE     2
#define BPF_PSEUDO_MAP_IDX_VALUE 6
#define BPF_PSEUDO_BTF_ID        3
#define BPF_PSEUDO_FUNC          4
#define BPF_PSEUDO_CALL          1
#define BPF_PSEUDO_KFUNC_CALL    2

#define BPF_ANY               0
#define BPF_NOEXIST           1
#define BPF_EXIST             2
#define BPF_F_LOCK            4
#define BPF_F_NO_PREALLOC     (1U << 0)
#define BPF_F_NO_COMMON_LRU   (1U << 1)
#define BPF_F_NUMA_NODE       (1U << 2)
#define BPF_F_RDONLY          (1U << 3)
#define BPF_F_WRONLY          (1U << 4)
#define BPF_F_STACK_BUILD_ID  (1U << 5)
#define BPF_F_ZERO_SEED       (1U << 6)
#define BPF_F_RDONLY_PROG     (1U << 7)
#define BPF_F_WRONLY_PROG     (1U << 8)
#define BPF_F_CLONE           (1U << 9)
#define BPF_F_MMAPABLE        (1U << 10)
#define BPF_F_PRESERVE_ELEMS  (1U << 11)
#define BPF_F_INNER_MAP       (1U << 12)
#define BPF_F_QUERY_EFFECTIVE (1U << 0)
#define BPF_F_TEST_RUN_ON_CPU (1U << 0)

#define BPF_STATS_RUN_TIME       0
#define BPF_STACK_BUILD_ID_EMPTY 0
#define BPF_STACK_BUILD_ID_VALID 1
#define BPF_STACK_BUILD_ID_IP    2
#define BPF_BUILD_ID_SIZE        20
#define BPF_OBJ_NAME_LEN         16U

#define BPF_F_RECOMPUTE_CSUM  (1ULL << 0)
#define BPF_F_INVALIDATE_HASH (1ULL << 1)
#define BPF_F_HDR_FIELD_MASK  0xfULL
#define BPF_F_PSEUDO_HDR      (1ULL << 4)
#define BPF_F_MARK_MANGLED_0  (1ULL << 5)
#define BPF_F_MARK_ENFORCE    (1ULL << 6)
#define BPF_F_INGRESS         (1ULL << 0)
#define BPF_F_TUNINFO_IPV6    (1ULL << 0)
#define BPF_F_SKIP_FIELD_MASK 0xffULL
#define BPF_F_USER_STACK      (1ULL << 8)
#define BPF_F_FAST_STACK_CMP  (1ULL << 9)
#define BPF_F_REUSE_STACKID   (1ULL << 10)
#define BPF_F_USER_BUILD_ID   (1ULL << 11)
#define BPF_F_ZERO_CSUM_TX    (1ULL << 1)
#define BPF_F_DONT_FRAGMENT   (1ULL << 2)
#define BPF_F_SEQ_NUMBER      (1ULL << 3)
#define BPF_F_INDEX_MASK      0xffffffffULL
#define BPF_F_CURRENT_CPU     BPF_F_INDEX_MASK
#define BPF_F_CTXLEN_MASK     (0xfffffULL << 32)
#define BPF_F_CURRENT_NETNS   (-1L)

#define BPF_CSUM_LEVEL_QUERY 0
#define BPF_CSUM_LEVEL_INC   1
#define BPF_CSUM_LEVEL_DEC   2
#define BPF_CSUM_LEVEL_RESET 3

#define BPF_F_ADJ_ROOM_FIXED_GSO     (1ULL << 0)
#define BPF_F_ADJ_ROOM_ENCAP_L3_IPV4 (1ULL << 1)
#define BPF_F_ADJ_ROOM_ENCAP_L3_IPV6 (1ULL << 2)
#define BPF_F_ADJ_ROOM_ENCAP_L4_GRE  (1ULL << 3)
#define BPF_F_ADJ_ROOM_ENCAP_L4_UDP  (1ULL << 4)
#define BPF_F_ADJ_ROOM_NO_CSUM_RESET (1ULL << 5)
#define BPF_F_ADJ_ROOM_ENCAP_L2_ETH  (1ULL << 6)
#define BPF_ADJ_ROOM_ENCAP_L2_MASK   0xff
#define BPF_ADJ_ROOM_ENCAP_L2_SHIFT  56

#define BPF_F_ADJ_ROOM_ENCAP_L2(len) \
  (((uint64_t)len & BPF_ADJ_ROOM_ENCAP_L2_MASK) << BPF_ADJ_ROOM_ENCAP_L2_SHIFT)

#define BPF_F_SYSCTL_BASE_NAME         (1ULL << 0)
#define BPF_LOCAL_STORAGE_GET_F_CREATE (1ULL << 0)
#define BPF_SK_STORAGE_GET_F_CREATE    BPF_LOCAL_STORAGE_GET_F_CREATE
#define BPF_F_GET_BRANCH_RECORDS_SIZE  (1ULL << 0)
#define BPF_RB_NO_WAKEUP               (1ULL << 0)
#define BPF_RB_FORCE_WAKEUP            (1ULL << 1)
#define BPF_RB_AVAIL_DATA              0
#define BPF_RB_RING_SIZE               1
#define BPF_RB_CONS_POS                2
#define BPF_RB_PROD_POS                3
#define BPF_RINGBUF_BUSY_BIT           (1U << 31)
#define BPF_RINGBUF_DISCARD_BIT        (1U << 30)
#define BPF_RINGBUF_HDR_SZ             8
#define BPF_SK_LOOKUP_F_REPLACE        (1ULL << 0)
#define BPF_SK_LOOKUP_F_NO_REUSEPORT   (1ULL << 1)

#define BPF_ADJ_ROOM_NET          0
#define BPF_ADJ_ROOM_MAC          1
#define BPF_HDR_START_MAC         0
#define BPF_HDR_START_NET         1
#define BPF_LWT_ENCAP_SEG6        0
#define BPF_LWT_ENCAP_SEG6_INLINE 1
#define BPF_LWT_ENCAP_IP          2
#define BPF_F_BPRM_SECUREEXEC     (1ULL << 0)
#define BPF_F_BROADCAST           (1ULL << 3)
#define BPF_F_EXCLUDE_INGRESS     (1ULL << 4)

#define XDP_PACKET_HEADROOM 256
#define BPF_TAG_SIZE        8

#define BPF_SOCK_OPS_RTO_CB_FLAG                   (1 << 0)
#define BPF_SOCK_OPS_RETRANS_CB_FLAG               (1 << 1)
#define BPF_SOCK_OPS_STATE_CB_FLAG                 (1 << 2)
#define BPF_SOCK_OPS_RTT_CB_FLAG                   (1 << 3)
#define BPF_SOCK_OPS_PARSE_ALL_HDR_OPT_CB_FLAG     (1 << 4)
#define BPF_SOCK_OPS_PARSE_UNKNOWN_HDR_OPT_CB_FLAG (1 << 5)
#define BPF_SOCK_OPS_WRITE_HDR_OPT_CB_FLAG         (1 << 6)
#define BPF_SOCK_OPS_ALL_CB_FLAGS                  0x7F

#define BPF_SOCK_OPS_VOID                   0
#define BPF_SOCK_OPS_TIMEOUT_INIT           1
#define BPF_SOCK_OPS_RWND_INIT              2
#define BPF_SOCK_OPS_TCP_CONNECT_CB         3
#define BPF_SOCK_OPS_ACTIVE_ESTABLISHED_CB  4
#define BPF_SOCK_OPS_PASSIVE_ESTABLISHED_CB 5
#define BPF_SOCK_OPS_NEEDS_ECN              6
#define BPF_SOCK_OPS_BASE_RTT               7
#define BPF_SOCK_OPS_RTO_CB                 8
#define BPF_SOCK_OPS_RETRANS_CB             9
#define BPF_SOCK_OPS_STATE_CB               10
#define BPF_SOCK_OPS_TCP_LISTEN_CB          11
#define BPF_SOCK_OPS_RTT_CB                 12
#define BPF_SOCK_OPS_PARSE_HDR_OPT_CB       13
#define BPF_SOCK_OPS_HDR_OPT_LEN_CB         14
#define BPF_SOCK_OPS_WRITE_HDR_OPT_CB       15

#define BPF_TCP_ESTABLISHED  1
#define BPF_TCP_SYN_SENT     2
#define BPF_TCP_SYN_RECV     3
#define BPF_TCP_FIN_WAIT1    4
#define BPF_TCP_FIN_WAIT2    5
#define BPF_TCP_TIME_WAIT    6
#define BPF_TCP_CLOSE        7
#define BPF_TCP_CLOSE_WAIT   8
#define BPF_TCP_LAST_ACK     9
#define BPF_TCP_LISTEN       10
#define BPF_TCP_CLOSING      11
#define BPF_TCP_NEW_SYN_RECV 12
#define BPF_TCP_MAX_STATES   13

#define TCP_BPF_IW                      1001
#define TCP_BPF_SNDCWND_CLAMP           1002
#define TCP_BPF_DELACK_MAX              1003
#define TCP_BPF_RTO_MIN                 1004
#define TCP_BPF_SYN                     1005
#define TCP_BPF_SYN_IP                  1006
#define TCP_BPF_SYN_MAC                 1007
#define BPF_LOAD_HDR_OPT_TCP_SYN        (1ULL << 0)
#define BPF_WRITE_HDR_TCP_CURRENT_MSS   1
#define BPF_WRITE_HDR_TCP_SYNACK_COOKIE 2

#define BPF_DEVCG_ACC_MKNOD (1ULL << 0)
#define BPF_DEVCG_ACC_READ  (1ULL << 1)
#define BPF_DEVCG_ACC_WRITE (1ULL << 2)
#define BPF_DEVCG_DEV_BLOCK (1ULL << 0)
#define BPF_DEVCG_DEV_CHAR  (1ULL << 1)

#define BPF_FIB_LOOKUP_DIRECT (1U << 0)
#define BPF_FIB_LOOKUP_OUTPUT (1U << 1)

#define BPF_FIB_LKUP_RET_SUCCESS      0
#define BPF_FIB_LKUP_RET_BLACKHOLE    1
#define BPF_FIB_LKUP_RET_UNREACHABLE  2
#define BPF_FIB_LKUP_RET_PROHIBIT     3
#define BPF_FIB_LKUP_RET_NOT_FWDED    4
#define BPF_FIB_LKUP_RET_FWD_DISABLED 5
#define BPF_FIB_LKUP_RET_UNSUPP_LWT   6
#define BPF_FIB_LKUP_RET_NO_NEIGH     7
#define BPF_FIB_LKUP_RET_FRAG_NEEDED  8

#define BPF_MTU_CHK_SEGS                        (1U << 0)
#define BPF_MTU_CHK_RET_SUCCESS                 0
#define BPF_MTU_CHK_RET_FRAG_NEEDED             1
#define BPF_MTU_CHK_RET_SEGS_TOOBIG             2
#define BPF_FD_TYPE_RAW_TRACEPOINT              0
#define BPF_FD_TYPE_TRACEPOINT                  1
#define BPF_FD_TYPE_KPROBE                      2
#define BPF_FD_TYPE_KRETPROBE                   3
#define BPF_FD_TYPE_UPROBE                      4
#define BPF_FD_TYPE_URETPROBE                   5
#define BPF_FLOW_DISSECTOR_F_PARSE_1ST_FRAG     (1U << 0)
#define BPF_FLOW_DISSECTOR_F_STOP_AT_FLOW_LABEL (1U << 1)
#define BPF_FLOW_DISSECTOR_F_STOP_AT_ENCAP      (1U << 2)

#define BPF_LINE_INFO_LINE_NUM(line_col) ((line_col) >> 10)
#define BPF_LINE_INFO_LINE_COL(line_col) ((line_col)&0x3ff)

#define BTF_F_COMPACT (1ULL << 0)
#define BTF_F_NONAME  (1ULL << 1)
#define BTF_F_PTR_RAW (1ULL << 2)
#define BTF_F_ZERO    (1ULL << 3)

#define BPF_OK          0
#define BPF_DROP        2
#define BPF_REDIRECT    7
#define BPF_LWT_REROUTE 128

#define BPF_FUNC_unspec                         0
#define BPF_FUNC_map_lookup_elem                1
#define BPF_FUNC_map_update_elem                2
#define BPF_FUNC_map_delete_elem                3
#define BPF_FUNC_probe_read                     4
#define BPF_FUNC_ktime_get_ns                   5
#define BPF_FUNC_trace_printk                   6
#define BPF_FUNC_get_prandom_u32                7
#define BPF_FUNC_get_smp_processor_id           8
#define BPF_FUNC_skb_store_bytes                9
#define BPF_FUNC_l3_csum_replace                10
#define BPF_FUNC_l4_csum_replace                11
#define BPF_FUNC_tail_call                      12
#define BPF_FUNC_clone_redirect                 13
#define BPF_FUNC_get_current_pid_tgid           14
#define BPF_FUNC_get_current_uid_gid            15
#define BPF_FUNC_get_current_comm               16
#define BPF_FUNC_get_cgroup_classid             17
#define BPF_FUNC_skb_vlan_push                  18
#define BPF_FUNC_skb_vlan_pop                   19
#define BPF_FUNC_skb_get_tunnel_key             20
#define BPF_FUNC_skb_set_tunnel_key             21
#define BPF_FUNC_perf_event_read                22
#define BPF_FUNC_redirect                       23
#define BPF_FUNC_get_route_realm                24
#define BPF_FUNC_perf_event_output              25
#define BPF_FUNC_skb_load_bytes                 26
#define BPF_FUNC_get_stackid                    27
#define BPF_FUNC_csum_diff                      28
#define BPF_FUNC_skb_get_tunnel_opt             29
#define BPF_FUNC_skb_set_tunnel_opt             30
#define BPF_FUNC_skb_change_proto               31
#define BPF_FUNC_skb_change_type                32
#define BPF_FUNC_skb_under_cgroup               33
#define BPF_FUNC_get_hash_recalc                34
#define BPF_FUNC_get_current_task               35
#define BPF_FUNC_probe_write_user               36
#define BPF_FUNC_current_task_under_cgroup      37
#define BPF_FUNC_skb_change_tail                38
#define BPF_FUNC_skb_pull_data                  39
#define BPF_FUNC_csum_update                    40
#define BPF_FUNC_set_hash_invalid               41
#define BPF_FUNC_get_numa_node_id               42
#define BPF_FUNC_skb_change_head                43
#define BPF_FUNC_xdp_adjust_head                44
#define BPF_FUNC_probe_read_str                 45
#define BPF_FUNC_get_socket_cookie              46
#define BPF_FUNC_get_socket_uid                 47
#define BPF_FUNC_set_hash                       48
#define BPF_FUNC_setsockopt                     49
#define BPF_FUNC_skb_adjust_room                50
#define BPF_FUNC_redirect_map                   51
#define BPF_FUNC_sk_redirect_map                52
#define BPF_FUNC_sock_map_update                53
#define BPF_FUNC_xdp_adjust_meta                54
#define BPF_FUNC_perf_event_read_value          55
#define BPF_FUNC_perf_prog_read_value           56
#define BPF_FUNC_getsockopt                     57
#define BPF_FUNC_override_return                58
#define BPF_FUNC_sock_ops_cb_flags_set          59
#define BPF_FUNC_msg_redirect_map               60
#define BPF_FUNC_msg_apply_bytes                61
#define BPF_FUNC_msg_cork_bytes                 62
#define BPF_FUNC_msg_pull_data                  63
#define BPF_FUNC_bind                           64
#define BPF_FUNC_xdp_adjust_tail                65
#define BPF_FUNC_skb_get_xfrm_state             66
#define BPF_FUNC_get_stack                      67
#define BPF_FUNC_skb_load_bytes_relative        68
#define BPF_FUNC_fib_lookup                     69
#define BPF_FUNC_sock_hash_update               70
#define BPF_FUNC_msg_redirect_hash              71
#define BPF_FUNC_sk_redirect_hash               72
#define BPF_FUNC_lwt_push_encap                 73
#define BPF_FUNC_lwt_seg6_store_bytes           74
#define BPF_FUNC_lwt_seg6_adjust_srh            75
#define BPF_FUNC_lwt_seg6_action                76
#define BPF_FUNC_rc_repeat                      77
#define BPF_FUNC_rc_keydown                     78
#define BPF_FUNC_skb_cgroup_id                  79
#define BPF_FUNC_get_current_cgroup_id          80
#define BPF_FUNC_get_local_storage              81
#define BPF_FUNC_sk_select_reuseport            82
#define BPF_FUNC_skb_ancestor_cgroup_id         83
#define BPF_FUNC_sk_lookup_tcp                  84
#define BPF_FUNC_sk_lookup_udp                  85
#define BPF_FUNC_sk_release                     86
#define BPF_FUNC_map_push_elem                  87
#define BPF_FUNC_map_pop_elem                   88
#define BPF_FUNC_map_peek_elem                  89
#define BPF_FUNC_msg_push_data                  90
#define BPF_FUNC_msg_pop_data                   91
#define BPF_FUNC_rc_pointer_rel                 92
#define BPF_FUNC_spin_lock                      93
#define BPF_FUNC_spin_unlock                    94
#define BPF_FUNC_sk_fullsock                    95
#define BPF_FUNC_tcp_sock                       96
#define BPF_FUNC_skb_ecn_set_ce                 97
#define BPF_FUNC_get_listener_sock              98
#define BPF_FUNC_skc_lookup_tcp                 99
#define BPF_FUNC_tcp_check_syncookie            100
#define BPF_FUNC_sysctl_get_name                101
#define BPF_FUNC_sysctl_get_current_value       102
#define BPF_FUNC_sysctl_get_new_value           103
#define BPF_FUNC_sysctl_set_new_value           104
#define BPF_FUNC_strtol                         105
#define BPF_FUNC_strtoul                        106
#define BPF_FUNC_sk_storage_get                 107
#define BPF_FUNC_sk_storage_delete              108
#define BPF_FUNC_send_signal                    109
#define BPF_FUNC_tcp_gen_syncookie              110
#define BPF_FUNC_skb_output                     111
#define BPF_FUNC_probe_read_user                112
#define BPF_FUNC_probe_read_kernel              113
#define BPF_FUNC_probe_read_user_str            114
#define BPF_FUNC_probe_read_kernel_str          115
#define BPF_FUNC_tcp_send_ack                   116
#define BPF_FUNC_send_signal_thread             117
#define BPF_FUNC_jiffies64                      118
#define BPF_FUNC_read_branch_records            119
#define BPF_FUNC_get_ns_current_pid_tgid        120
#define BPF_FUNC_xdp_output                     121
#define BPF_FUNC_get_netns_cookie               122
#define BPF_FUNC_get_current_ancestor_cgroup_id 123
#define BPF_FUNC_sk_assign                      124
#define BPF_FUNC_ktime_get_boot_ns              125
#define BPF_FUNC_seq_printf                     126
#define BPF_FUNC_seq_write                      127
#define BPF_FUNC_sk_cgroup_id                   128
#define BPF_FUNC_sk_ancestor_cgroup_id          129
#define BPF_FUNC_ringbuf_output                 130
#define BPF_FUNC_ringbuf_reserve                131
#define BPF_FUNC_ringbuf_submit                 132
#define BPF_FUNC_ringbuf_discard                133
#define BPF_FUNC_ringbuf_query                  134
#define BPF_FUNC_csum_level                     135
#define BPF_FUNC_skc_to_tcp6_sock               136
#define BPF_FUNC_skc_to_tcp_sock                137
#define BPF_FUNC_skc_to_tcp_timewait_sock       138
#define BPF_FUNC_skc_to_tcp_request_sock        139
#define BPF_FUNC_skc_to_udp6_sock               140
#define BPF_FUNC_get_task_stack                 141
#define BPF_FUNC_load_hdr_opt                   142
#define BPF_FUNC_store_hdr_opt                  143
#define BPF_FUNC_reserve_hdr_opt                144
#define BPF_FUNC_inode_storage_get              145
#define BPF_FUNC_inode_storage_delete           146
#define BPF_FUNC_d_path                         147
#define BPF_FUNC_copy_from_user                 148
#define BPF_FUNC_snprintf_btf                   149
#define BPF_FUNC_seq_printf_btf                 150
#define BPF_FUNC_skb_cgroup_classid             151
#define BPF_FUNC_redirect_neigh                 152
#define BPF_FUNC_per_cpu_ptr                    153
#define BPF_FUNC_this_cpu_ptr                   154
#define BPF_FUNC_redirect_peer                  155
#define BPF_FUNC_task_storage_get               156
#define BPF_FUNC_task_storage_delete            157
#define BPF_FUNC_get_current_task_btf           158
#define BPF_FUNC_bprm_opts_set                  159
#define BPF_FUNC_ktime_get_coarse_ns            160
#define BPF_FUNC_ima_inode_hash                 161
#define BPF_FUNC_sock_from_file                 162
#define BPF_FUNC_check_mtu                      163
#define BPF_FUNC_for_each_map_elem              164
#define BPF_FUNC_snprintf                       165
#define BPF_FUNC_sys_bpf                        166
#define BPF_FUNC_btf_find_by_name_kind          167
#define BPF_FUNC_sys_close                      168
#define __BPF_FUNC_MAX_ID                       169

#define __bpf_md_ptr(type, name) \
  union {                        \
    type name;                   \
    uint64_t : 64;               \
  } forcealign(8)

struct bpf_insn {
  uint8_t code;
  uint8_t dst_reg : 4;
  uint8_t src_reg : 4;
  int16_t off;
  int32_t imm;
};

struct bpf_lpm_trie_key {
  uint32_t prefixlen;
  uint8_t data[0];
};

struct bpf_cgroup_storage_key {
  uint64_t cgroup_inode_id;
  uint32_t attach_type;
};

union bpf_iter_link_info {
  struct {
    uint32_t map_fd;
  } map;
};

struct bpf_stack_build_id {
  int32_t status;
  unsigned char build_id[BPF_BUILD_ID_SIZE];
  union {
    uint64_t offset;
    uint64_t ip;
  };
};

union bpf_attr {
  struct {
    uint32_t map_type;
    uint32_t key_size;
    uint32_t value_size;
    uint32_t max_entries;
    uint32_t map_flags;
    uint32_t inner_map_fd;
    uint32_t numa_node;
    char map_name[BPF_OBJ_NAME_LEN];
    uint32_t map_ifindex;
    uint32_t btf_fd;
    uint32_t btf_key_type_id;
    uint32_t btf_value_type_id;
    uint32_t btf_vmlinux_value_type_id;
  };
  struct {
    uint32_t map_fd;
    uint64_t key;
    union {
      uint64_t value;
      uint64_t next_key;
    };
    uint64_t flags;
  };
  struct {
    uint64_t in_batch;
    uint64_t out_batch;
    uint64_t keys;
    uint64_t values;
    uint32_t count;
    uint32_t map_fd;
    uint64_t elem_flags;
    uint64_t flags;
  } batch;
  struct {
    uint32_t prog_type;
    uint32_t insn_cnt;
    uint64_t insns;
    uint64_t license;
    uint32_t log_level;
    uint32_t log_size;
    uint64_t log_buf;
    uint32_t kern_version;
    uint32_t prog_flags;
    char prog_name[BPF_OBJ_NAME_LEN];
    uint32_t prog_ifindex;
    uint32_t expected_attach_type;
    uint32_t prog_btf_fd;
    uint32_t func_info_rec_size;
    uint64_t func_info;
    uint32_t func_info_cnt;
    uint32_t line_info_rec_size;
    uint64_t line_info;
    uint32_t line_info_cnt;
    uint32_t attach_btf_id;
    union {
      uint32_t attach_prog_fd;
      uint32_t attach_btf_obj_fd;
    };
    uint32_t : 32;
    uint64_t fd_array;
  };
  struct {
    uint64_t pathname;
    uint32_t bpf_fd;
    uint32_t file_flags;
  };
  struct {
    uint32_t target_fd;
    uint32_t attach_bpf_fd;
    uint32_t attach_type;
    uint32_t attach_flags;
    uint32_t replace_bpf_fd;
  };
  struct {
    uint32_t prog_fd;
    uint32_t retval;
    uint32_t data_size_in;
    uint32_t data_size_out;
    uint64_t data_in;
    uint64_t data_out;
    uint32_t repeat;
    uint32_t duration;
    uint32_t ctx_size_in;
    uint32_t ctx_size_out;
    uint64_t ctx_in;
    uint64_t ctx_out;
    uint32_t flags;
    uint32_t cpu;
  } test;
  struct {
    union {
      uint32_t start_id;
      uint32_t prog_id;
      uint32_t map_id;
      uint32_t btf_id;
      uint32_t link_id;
    };
    uint32_t next_id;
    uint32_t open_flags;
  };
  struct {
    uint32_t bpf_fd;
    uint32_t info_len;
    uint64_t info;
  } info;
  struct {
    uint32_t target_fd;
    uint32_t attach_type;
    uint32_t query_flags;
    uint32_t attach_flags;
    uint64_t prog_ids;
    uint32_t prog_cnt;
  } query;
  struct {
    uint64_t name;
    uint32_t prog_fd;
  } raw_tracepoint;
  struct {
    uint64_t btf;
    uint64_t btf_log_buf;
    uint32_t btf_size;
    uint32_t btf_log_size;
    uint32_t btf_log_level;
  };
  struct {
    uint32_t pid;
    uint32_t fd;
    uint32_t flags;
    uint32_t buf_len;
    uint64_t buf;
    uint32_t prog_id;
    uint32_t fd_type;
    uint64_t probe_offset;
    uint64_t probe_addr;
  } task_fd_query;
  struct {
    uint32_t prog_fd;
    union {
      uint32_t target_fd;
      uint32_t target_ifindex;
    };
    uint32_t attach_type;
    uint32_t flags;
    union {
      uint32_t target_btf_id;
      struct {
        uint64_t iter_info;
        uint32_t iter_info_len;
      };
    };
  } link_create;
  struct {
    uint32_t link_fd;
    uint32_t new_prog_fd;
    uint32_t flags;
    uint32_t old_prog_fd;
  } link_update;
  struct {
    uint32_t link_fd;
  } link_detach;
  struct {
    uint32_t type;
  } enable_stats;
  struct {
    uint32_t link_fd;
    uint32_t flags;
  } iter_create;
  struct {
    uint32_t prog_fd;
    uint32_t map_fd;
    uint32_t flags;
  } prog_bind_map;
} forcealign(8);

struct __sk_buff {
  uint32_t len;
  uint32_t pkt_type;
  uint32_t mark;
  uint32_t queue_mapping;
  uint32_t protocol;
  uint32_t vlan_present;
  uint32_t vlan_tci;
  uint32_t vlan_proto;
  uint32_t priority;
  uint32_t ingress_ifindex;
  uint32_t ifindex;
  uint32_t tc_index;
  uint32_t cb[5];
  uint32_t hash;
  uint32_t tc_classid;
  uint32_t data;
  uint32_t data_end;
  uint32_t napi_id;
  uint32_t family;
  uint32_t remote_ip4;
  uint32_t local_ip4;
  uint32_t remote_ip6[4];
  uint32_t local_ip6[4];
  uint32_t remote_port;
  uint32_t local_port;
  uint32_t data_meta;
  __bpf_md_ptr(struct bpf_flow_keys *, flow_keys);
  uint64_t tstamp;
  uint32_t wire_len;
  uint32_t gso_segs;
  __bpf_md_ptr(struct bpf_sock *, sk);
  uint32_t gso_size;
};

struct bpf_tunnel_key {
  uint32_t tunnel_id;
  union {
    uint32_t remote_ipv4;
    uint32_t remote_ipv6[4];
  };
  uint8_t tunnel_tos;
  uint8_t tunnel_ttl;
  uint16_t tunnel_ext;
  uint32_t tunnel_label;
};

struct bpf_xfrm_state {
  uint32_t reqid;
  uint32_t spi;
  uint16_t family;
  uint16_t ext;
  union {
    uint32_t remote_ipv4;
    uint32_t remote_ipv6[4];
  };
};

struct bpf_sock {
  uint32_t bound_dev_if;
  uint32_t family;
  uint32_t type;
  uint32_t protocol;
  uint32_t mark;
  uint32_t priority;
  uint32_t src_ip4;
  uint32_t src_ip6[4];
  uint32_t src_port;
  uint32_t dst_port;
  uint32_t dst_ip4;
  uint32_t dst_ip6[4];
  uint32_t state;
  int32_t rx_queue_mapping;
};

struct bpf_tcp_sock {
  uint32_t snd_cwnd;
  uint32_t srtt_us;
  uint32_t rtt_min;
  uint32_t snd_ssthresh;
  uint32_t rcv_nxt;
  uint32_t snd_nxt;
  uint32_t snd_una;
  uint32_t mss_cache;
  uint32_t ecn_flags;
  uint32_t rate_delivered;
  uint32_t rate_interval_us;
  uint32_t packets_out;
  uint32_t retrans_out;
  uint32_t total_retrans;
  uint32_t segs_in;
  uint32_t data_segs_in;
  uint32_t segs_out;
  uint32_t data_segs_out;
  uint32_t lost_out;
  uint32_t sacked_out;
  uint64_t bytes_received;
  uint64_t bytes_acked;
  uint32_t dsack_dups;
  uint32_t delivered;
  uint32_t delivered_ce;
  uint32_t icsk_retransmits;
};

struct bpf_sock_tuple {
  union {
    struct {
      uint32_t saddr; /* big endian */
      uint32_t daddr; /* big endian */
      uint16_t sport; /* big endian */
      uint16_t dport; /* big endian */
    } ipv4;
    struct {
      uint32_t saddr[4]; /* big endian */
      uint32_t daddr[4]; /* big endian */
      uint16_t sport;    /* big endian */
      uint16_t dport;    /* big endian */
    } ipv6;
  };
};

struct bpf_xdp_sock {
  uint32_t queue_id;
};

enum xdp_action {
  XDP_ABORTED = 0,
  XDP_DROP,
  XDP_PASS,
  XDP_TX,
  XDP_REDIRECT,
};

struct xdp_md {
  uint32_t data;
  uint32_t data_end;
  uint32_t data_meta;
  uint32_t ingress_ifindex;
  uint32_t rx_queue_index;
  uint32_t egress_ifindex;
};

struct bpf_devmap_val {
  uint32_t ifindex;
  union {
    int fd;
    uint32_t id;
  } bpf_prog;
};

struct bpf_cpumap_val {
  uint32_t qsize;
  union {
    int fd;
    uint32_t id;
  } bpf_prog;
};

enum sk_action {
  SK_DROP = 0,
  SK_PASS,
};

struct sk_msg_md {
  __bpf_md_ptr(void *, data);
  __bpf_md_ptr(void *, data_end);
  uint32_t family;
  uint32_t remote_ip4;
  uint32_t local_ip4;
  uint32_t remote_ip6[4];
  uint32_t local_ip6[4];
  uint32_t remote_port;
  uint32_t local_port;
  uint32_t size;
  __bpf_md_ptr(struct bpf_sock *, sk);
};

struct sk_reuseport_md {
  __bpf_md_ptr(void *, data);
  __bpf_md_ptr(void *, data_end);
  uint32_t len;
  uint32_t eth_protocol;
  uint32_t ip_protocol;
  uint32_t bind_inany;
  uint32_t hash;
  __bpf_md_ptr(struct bpf_sock *, sk);
  __bpf_md_ptr(struct bpf_sock *, migrating_sk);
};

struct bpf_prog_info {
  uint32_t type;
  uint32_t id;
  uint8_t tag[BPF_TAG_SIZE];
  uint32_t jited_prog_len;
  uint32_t xlated_prog_len;
  uint64_t jited_prog_insns;
  uint64_t xlated_prog_insns;
  uint64_t load_time;
  uint32_t created_by_uid;
  uint32_t nr_map_ids;
  uint64_t map_ids;
  char name[BPF_OBJ_NAME_LEN];
  uint32_t ifindex;
  uint32_t gpl_compatible : 1;
  uint32_t : 31;
  uint64_t netns_dev;
  uint64_t netns_ino;
  uint32_t nr_jited_ksyms;
  uint32_t nr_jited_func_lens;
  uint64_t jited_ksyms;
  uint64_t jited_func_lens;
  uint32_t btf_id;
  uint32_t func_info_rec_size;
  uint64_t func_info;
  uint32_t nr_func_info;
  uint32_t nr_line_info;
  uint64_t line_info;
  uint64_t jited_line_info;
  uint32_t nr_jited_line_info;
  uint32_t line_info_rec_size;
  uint32_t jited_line_info_rec_size;
  uint32_t nr_prog_tags;
  uint64_t prog_tags;
  uint64_t run_time_ns;
  uint64_t run_cnt;
  uint64_t recursion_misses;
} forcealign(8);

struct bpf_map_info {
  uint32_t type;
  uint32_t id;
  uint32_t key_size;
  uint32_t value_size;
  uint32_t max_entries;
  uint32_t map_flags;
  char name[BPF_OBJ_NAME_LEN];
  uint32_t ifindex;
  uint32_t btf_vmlinux_value_type_id;
  uint64_t netns_dev;
  uint64_t netns_ino;
  uint32_t btf_id;
  uint32_t btf_key_type_id;
  uint32_t btf_value_type_id;
} forcealign(8);

struct bpf_btf_info {
  uint64_t btf;
  uint32_t btf_size;
  uint32_t id;
  uint64_t name;
  uint32_t name_len;
  uint32_t kernel_btf;
} forcealign(8);

struct bpf_link_info {
  uint32_t type;
  uint32_t id;
  uint32_t prog_id;
  union {
    struct {
      uint64_t tp_name;
      uint32_t tp_name_len;
    } raw_tracepoint;
    struct {
      uint32_t attach_type;
      uint32_t target_obj_id;
      uint32_t target_btf_id;
    } tracing;
    struct {
      uint64_t cgroup_id;
      uint32_t attach_type;
    } cgroup;
    struct {
      uint64_t target_name;
      uint32_t target_name_len;
      union {
        struct {
          uint32_t map_id;
        } map;
      };
    } iter;
    struct {
      uint32_t netns_ino;
      uint32_t attach_type;
    } netns;
    struct {
      uint32_t ifindex;
    } xdp;
  };
} forcealign(8);

struct bpf_sock_addr {
  uint32_t user_family;
  uint32_t user_ip4;
  uint32_t user_ip6[4];
  uint32_t user_port;
  uint32_t family;
  uint32_t type;
  uint32_t protocol;
  uint32_t msg_src_ip4;
  uint32_t msg_src_ip6[4];
  __bpf_md_ptr(struct bpf_sock *, sk);
};

struct bpf_sock_ops {
  uint32_t op;
  union {
    uint32_t args[4];
    uint32_t reply;
    uint32_t replylong[4];
  };
  uint32_t family;
  uint32_t remote_ip4;
  uint32_t local_ip4;
  uint32_t remote_ip6[4];
  uint32_t local_ip6[4];
  uint32_t remote_port;
  uint32_t local_port;
  uint32_t is_fullsock;
  uint32_t snd_cwnd;
  uint32_t srtt_us;
  uint32_t bpf_sock_ops_cb_flags;
  uint32_t state;
  uint32_t rtt_min;
  uint32_t snd_ssthresh;
  uint32_t rcv_nxt;
  uint32_t snd_nxt;
  uint32_t snd_una;
  uint32_t mss_cache;
  uint32_t ecn_flags;
  uint32_t rate_delivered;
  uint32_t rate_interval_us;
  uint32_t packets_out;
  uint32_t retrans_out;
  uint32_t total_retrans;
  uint32_t segs_in;
  uint32_t data_segs_in;
  uint32_t segs_out;
  uint32_t data_segs_out;
  uint32_t lost_out;
  uint32_t sacked_out;
  uint32_t sk_txhash;
  uint64_t bytes_received;
  uint64_t bytes_acked;
  __bpf_md_ptr(struct bpf_sock *, sk);
  __bpf_md_ptr(void *, skb_data);
  __bpf_md_ptr(void *, skb_data_end);
  uint32_t skb_len;
  uint32_t skb_tcp_flags;
};

struct bpf_perf_event_value {
  uint64_t counter;
  uint64_t enabled;
  uint64_t running;
};

struct bpf_cgroup_dev_ctx {
  uint32_t access_type;
  uint32_t major;
  uint32_t minor;
};

struct bpf_raw_tracepoint_args {
  uint64_t args[0];
};

struct bpf_fib_lookup {
  uint8_t family;
  uint8_t l4_protocol;
  uint16_t sport; /* big endian */
  uint16_t dport; /* big endian */
  union {
    uint16_t tot_len;
    uint16_t mtu_result;
  };
  uint32_t ifindex;
  union {
    uint8_t tos;
    uint32_t flowinfo; /* big endian */
    uint32_t rt_metric;
  };
  union {
    uint32_t ipv4_src; /* big endian */
    uint32_t ipv6_src[4];
  };
  union {
    uint32_t ipv4_dst; /* big endian */
    uint32_t ipv6_dst[4];
  };
  uint16_t h_vlan_proto; /* big endian */
  uint16_t h_vlan_TCI;   /* big endian */
  uint8_t smac[6];
  uint8_t dmac[6];
};

struct bpf_redir_neigh {
  uint32_t nh_family;
  union {
    uint32_t ipv4_nh; /* big endian */
    uint32_t ipv6_nh[4];
  };
};

struct bpf_flow_keys {
  uint16_t nhoff;
  uint16_t thoff;
  uint16_t addr_proto;
  uint8_t is_frag;
  uint8_t is_first_frag;
  uint8_t is_encap;
  uint8_t ip_proto;
  uint16_t n_proto; /* big endian */
  uint16_t sport;   /* big endian */
  uint16_t dport;   /* big endian */
  union {
    struct {
      uint32_t ipv4_src; /* big endian */
      uint32_t ipv4_dst; /* big endian */
    };
    struct {
      uint32_t ipv6_src[4];
      uint32_t ipv6_dst[4];
    };
  };
  uint32_t flags;
  uint32_t flow_label;
};

struct bpf_func_info {
  uint32_t insn_off;
  uint32_t type_id;
};

struct bpf_line_info {
  uint32_t insn_off;
  uint32_t file_name_off;
  uint32_t line_off;
  uint32_t line_col;
};

struct bpf_spin_lock {
  uint32_t val;
};

struct bpf_sysctl {
  uint32_t write_;
  uint32_t file_pos;
};

struct bpf_sockopt {
  __bpf_md_ptr(struct bpf_sock *, sk);
  __bpf_md_ptr(void *, optval);
  __bpf_md_ptr(void *, optval_end);
  int32_t level;
  int32_t optname;
  int32_t optlen;
  int32_t retval;
};

struct bpf_pidns_info {
  uint32_t pid;
  uint32_t tgid;
};

struct bpf_sk_lookup {
  union {
    __bpf_md_ptr(struct bpf_sock *, sk);
    uint64_t cookie;
  };
  uint32_t family;
  uint32_t protocol;
  uint32_t remote_ip4;
  uint32_t remote_ip6[4];
  uint32_t remote_port;
  uint32_t local_ip4;
  uint32_t local_ip6[4];
  uint32_t local_port;
};

struct btf_ptr {
  void *ptr;
  uint32_t type_id;
  uint32_t flags;
};

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_BPF_H_ */
