#ifndef COSMOPOLITAN_LIBC_TESTLIB_TRACE_H_
#define COSMOPOLITAN_LIBC_TESTLIB_TRACE_H_
COSMOPOLITAN_C_START_

void cosmo_trace_set_pid(int);
void cosmo_trace_set_tid(int);
void cosmo_trace_begin(const char*);
void cosmo_trace_end(const char*);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_TESTLIB_TRACE_H_ */
