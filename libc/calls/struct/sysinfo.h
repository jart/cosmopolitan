#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SYSINFO_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SYSINFO_H_
COSMOPOLITAN_C_START_

struct sysinfo {
  int64_t uptime;     /* seconds since boot */
  uint64_t loads[3];  /* 1-5-15 min active process averages */
  uint64_t totalram;  /* system physical memory */
  uint64_t freeram;   /* amount of ram currently going to waste */
  uint64_t sharedram; /* bytes w/ pages mapped into multiple progs */
  uint64_t bufferram; /* lingering disk pages; see fadvise */
  uint64_t totalswap; /* size of emergency memory */
  uint64_t freeswap;  /* hopefully equal to totalswap */
  uint16_t procs;     /* number of processes */
  int16_t __ignore1;  /* padding */
  int32_t __ignore2;  /* padding */
  uint64_t totalhigh; /* wut */
  uint64_t freehigh;  /* wut */
  uint32_t mem_unit;  /* ram stuff above is multiples of this */
};

int sysinfo(struct sysinfo *) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SYSINFO_H_ */
