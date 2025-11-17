#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_RSEQ_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_RSEQ_H_
#include "libc/thread/tls.h"

#define RSEQ_CPU_ID_UNINITIALIZED       -1
#define RSEQ_CPU_ID_REGISTRATION_FAILED -2

#define RSEQ_FLAG_UNREGISTER 1

#define RSEQ_CS_FLAG_NO_RESTART_ON_PREEMPT 1 /* deprecated */
#define RSEQ_CS_FLAG_NO_RESTART_ON_SIGNAL  2 /* deprecated */
#define RSEQ_CS_FLAG_NO_RESTART_ON_MIGRATE 4 /* deprecated */

#ifdef __x86_64__
#define RSEQ_SIG 0x53053053 /* ud1 0x53053053(%rip),%edi */
#elif defined(__aarch64__)
#define RSEQ_SIG 0xd428bc00 /* brk #0x45e0 */
#endif

/* consider putting this in the .rodata.rseq section of your binary */
struct rseq_cs {
  uint32_t version;
  uint32_t flags;
  uint64_t start_ip;
  uint64_t post_commit_offset;
  uint64_t abort_ip;
} forcealign(32);

/* this memory should go in thread local storage */
struct rseq {

  /* The difference between the two is that `cpu_id_start` is always in
     range, whereas `cpu_id` may contain error values. The kernel
     provides both in order to support computation of values derived
     from the CPU ID that happens before entering the critical section.
     We could do this with one CPU ID, but it would require an extra
     branch to distinguish "not initialized" from "CPU ID changed after
     fetching it". On the other hand if (like tcmalloc) you only fetch
     the CPU Id within a critical section, then you need only one field
     because you have only one branch: am I initialized. There is no
     such thing as a CPU mismatch because instead you are just restarted
     when the CPU ID changes. —Quoth tcmalloc (rseq.md) */
  uint32_t cpu_id_start;

  /* this should be initialized to RSEQ_CPU_ID_UNINITIALIZED when this
     data structure is registered with the kernel. you'll then be able
     to test if the host OS is Linux 4.18+ by simply checking cpu_id≥0 */
  volatile int cpu_id;

  /* when this points to `struct rseq_cs` the kernel will be advised on
     how to handle the interrupting of this thread */
  uint64_t rseq_cs;

  uint32_t flags;
  uint32_t node_id;
  uint32_t mm_cid;
  char end[];
} forcealign(32);

#define __get_rseq() ((struct rseq *)__get_tls()->tib_rseq)

#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_RSEQ_H_ */
