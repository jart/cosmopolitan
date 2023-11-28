#ifndef COSMOPOLITAN_LIBC_CALLS_CACHESTAT_H_
#define COSMOPOLITAN_LIBC_CALLS_CACHESTAT_H_

COSMOPOLITAN_C_START_

struct cachestat_range {
  uint64_t off;
  uint64_t len;
};

struct cachestat {
  /** Number of cached pages. */
  uint64_t nr_cache;
  /** Number of dirty pages */
  uint64_t nr_dirty;
  /** Number of pages marked for writeback. */
  uint64_t nr_writeback;
  /** Number of pages evicted from the cache. */
  uint64_t nr_evicted;
  /**
   * Number of recently evicted pages.
   * A page is recently evicted if its last eviction was recent enough that its
   * reentry to the cache would indicate that it is actively being used by the
   * system, and that there is memory pressure on the system.
   */
  uint64_t nr_recently_evicted;
};

int cachestat(int, struct cachestat_range *, struct cachestat *, uint32_t);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_LANDLOCK_H_ */