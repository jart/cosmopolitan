#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SYSINFO_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SYSINFO_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct sysinfo {
  int64_t uptime;     /* seconds since boot */
  uint64_t loads[3];  /* 1-5-15 min active process averages */
  uint64_t totalram;  /* system physical memory */
  uint64_t freeram;   /* amount of ram currently going to waste */
  uint64_t sharedram; /* bytes w/ pages mapped into multiple progs */
  uint64_t bufferram; /* lingering disk pages; see fadvise */
  uint64_t totalswap; /* size of emergency memory */
  uint64_t freeswap;  /* hopefully equal to totalswap */
  int16_t procs;      /* number of processes */
  int16_t __ignore;   /* padding */
  uint64_t totalhigh; /* wut */
  uint64_t freehigh;  /* wut */
  uint32_t mem_unit;  /* ram stuff above is multiples of this */
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SYSINFO_H_ */
