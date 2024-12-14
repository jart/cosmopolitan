/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "trace.h"
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <threads.h>
#include <unistd.h>

struct TraceEvent {
  unsigned long long ts;
  int pid;
  int tid;
  const char* name;
  const char* cat;
  char ph;
};

static int g_pid;
static atomic_bool g_oom;
static atomic_int g_count;
static thread_local int g_id;
static thread_local int g_ids;
static thread_local int g_tid;
static unsigned long g_start_rdtsc;
static struct TraceEvent g_events[1000000];

static unsigned long rdtsc(void) {
#ifdef __x86_64__
  unsigned ax, dx;
  __asm__ volatile("rdtsc" : "=a"(ax), "=d"(dx));
  return (unsigned long)dx << 32 | ax;
#else
  unsigned long c;
  __asm__ volatile("mrs %0, cntvct_el0" : "=r"(c));
  return c * 48;  // the fudge factor
#endif
}

static int cosmo_trace_oom(void) {
  if (atomic_load_explicit(&g_oom, memory_order_relaxed))
    return -1;
  if (atomic_exchange_explicit(&g_oom, true, memory_order_acq_rel))
    return -1;
  fprintf(stderr, "warning: ran out of trace event memory\n");
  return -1;
}

static int cosmo_trace_reserve(int count) {
  int id = atomic_load_explicit(&g_count, memory_order_relaxed);
  if (id + count > sizeof(g_events) / sizeof(*g_events))
    return cosmo_trace_oom();
  id = atomic_fetch_add_explicit(&g_count, count, memory_order_acq_rel);
  if (id + count > sizeof(g_events) / sizeof(*g_events))
    return cosmo_trace_oom();
  return id;
}

static void cosmo_trace_event(int id, const char* name, const char* cat,
                              char ph) {
  g_events[id].ts = rdtsc();
  g_events[id].pid = g_pid ? g_pid - 1 : getpid();
  g_events[id].tid = g_tid ? g_tid - 1 : gettid();
  g_events[id].name = name;
  g_events[id].cat = cat;
  g_events[id].ph = ph;
}

void cosmo_trace_set_pid(int pid) {
  g_pid = pid + 1;
}

void cosmo_trace_set_tid(int tid) {
  g_tid = tid + 1;
}

void cosmo_trace_begin(const char* name) {
  if (g_ids < 2) {
    g_ids = 20;
    g_id = cosmo_trace_reserve(g_ids);
    if (g_id == -1) {
      g_ids = 0;
      return;
    }
  }
  cosmo_trace_event(g_id++, name, "category", 'B');
  --g_ids;
}

void cosmo_trace_end(const char* name) {
  if (g_ids < 1)
    return;
  cosmo_trace_event(g_id++, name, "category", 'E');
  --g_ids;
}

static void cosmo_trace_save(const char* filename) {
  int count = atomic_load_explicit(&g_count, memory_order_relaxed);
  if (!count)
    return;
  fprintf(stderr, "saving trace to %s...\n", filename);
  FILE* file = fopen(filename, "w");
  if (!file) {
    perror(filename);
    return;
  }
  fprintf(file, "[\n");
  bool once = false;
  for (int i = 0; i < count; i++) {
    if (!g_events[i].name)
      continue;
    if (!once) {
      once = true;
    } else {
      fputs(",\n", file);
    }
    fprintf(file,
            "{\"name\": \"%s\", \"cat\": \"%s\", \"ph\": \"%c\", "
            "\"ts\": %.3f, \"pid\": %d, \"tid\": %d}",
            g_events[i].name, g_events[i].cat, g_events[i].ph,
            (g_events[i].ts - g_start_rdtsc) / 3000., g_events[i].pid,
            g_events[i].tid);
  }
  fprintf(file, "\n]\n");
  fclose(file);
}

__attribute__((__constructor__)) static void trace_startup(void) {
  g_start_rdtsc = rdtsc();
}

__attribute__((__destructor__)) static void trace_shutdown(void) {
  cosmo_trace_save("trace.json");  // see chrome://tracing/
}
