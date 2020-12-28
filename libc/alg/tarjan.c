/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/alg/alg.h"
#include "libc/assert.h"
#include "libc/limits.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"

/**
 * @fileoverview Tarjan's Strongly Connected Components Algorithm.
 *
 *    “The data structures that [Tarjan] devised for this problem fit
 *     together in an amazingly beautiful way, so that the quantities
 *     you need to look at while exploring a directed graph are always
 *     magically at your fingertips. And his algorithm also does
 *     topological sorting as a byproduct.” ──D.E. Knuth
 */

struct Tarjan {
  int Vn, En, Ci, Ri, *R, *C, index;
  const int (*E)[2];
  struct Vertex {
    int Vi;
    int Ei;
    int index;
    int lowlink;
    bool onstack;
    bool selfreferential;
  } * V;
  struct TarjanStack {
    int i;
    int n;
    int *p;
  } S;
};

static bool TarjanPush(struct Tarjan *t, int v) {
  int *q;
  assert(t->S.i >= 0);
  assert(t->S.n >= 0);
  assert(0 <= v && v < t->Vn);
  if (t->S.i == t->S.n) {
    if ((q = realloc(t->S.p, (t->S.n + (t->S.n >> 1) + 8) * sizeof(*t->S.p)))) {
      t->S.p = q;
    } else {
      return false;
    }
  }
  t->S.p[t->S.i++] = v;
  return true;
}

static int TarjanPop(struct Tarjan *t) {
  assert(t->S.i > 0);
  return t->S.p[--t->S.i];
}

static bool TarjanConnect(struct Tarjan *t, int v) {
  int fs, w, e;
  assert(0 <= v && v < t->Vn);
  t->V[v].index = t->index;
  t->V[v].lowlink = t->index;
  t->V[v].onstack = true;
  t->index++;
  if (!TarjanPush(t, v)) return false;
  fs = t->V[v].Ei;
  if (fs != -1) {
    for (e = fs; e < t->En && v == t->E[e][0]; ++e) {
      w = t->E[e][1];
      if (!t->V[w].index) {
        if (!TarjanConnect(t, t->V[w].Vi)) return false;
        t->V[v].lowlink = MIN(t->V[v].lowlink, t->V[w].lowlink);
      } else if (t->V[w].onstack) {
        t->V[v].lowlink = MIN(t->V[v].lowlink, t->V[w].index);
      }
      if (w == v) {
        t->V[w].selfreferential = true;
      }
    }
  }
  if (t->V[v].lowlink == t->V[v].index) {
    do {
      w = TarjanPop(t);
      t->V[w].onstack = false;
      t->R[t->Ri++] = t->V[w].Vi;
    } while (w != v);
    if (t->C) t->C[t->Ci++] = t->Ri;
  }
  return true;
}

/**
 * Determines order of things in network and finds tangled clusters too.
 *
 * @param vertices is an array of vertex values, which isn't passed to
 *     this function, since the algorithm only needs to consider indices
 * @param vertex_count is the number of items in the vertices array
 * @param edges are grouped directed links between indices of vertices,
 *     which can be thought of as "edge[i][0] depends on edge[i][1]" or
 *     "edge[i][1] must come before edge[i][0]" in topological order
 * @param edge_count is the number of items in edges, which may be 0 if
 *     there aren't any connections between vertices in the graph
 * @param out_sorted receives indices into the vertices array in
 *     topologically sorted order, and must be able to store
 *     vertex_count items, and that's always how many are stored
 * @param out_opt_components receives indices into the out_sorted array,
 *     indicating where each strongly-connected component ends; must be
 *     able to store vertex_count items; and it may be NULL
 * @param out_opt_componentcount receives the number of cycle indices
 *     written to out_opt_components, which will be vertex_count if
 *     there aren't any cycles in the graph; and may be NULL if
 *     out_opt_components is NULL
 * @return 0 on success or -1 w/ errno
 * @error ENOMEM
 * @note Tarjan's Algorithm is O(|V|+|E|)
 */
int tarjan(int vertex_count, const int (*edges)[2], int edge_count,
           int out_sorted[], int out_opt_components[],
           int *out_opt_componentcount) {
  int i, rc, v, e;
  struct Tarjan *t;
  assert(0 <= edge_count && edge_count <= INT_MAX);
  assert(0 <= vertex_count && vertex_count <= INT_MAX);
  for (i = 0; i < edge_count; ++i) {
    if (i) assert(edges[i - 1][0] <= edges[i][0]);
    assert(edges[i][0] < vertex_count);
    assert(edges[i][1] < vertex_count);
  }
  if (!(t = calloc(1, (sizeof(struct Tarjan) +
                       sizeof(struct Vertex) * vertex_count)))) {
    return -1;
  }
  t->V = (struct Vertex *)((char *)t + sizeof(struct Tarjan));
  t->Vn = vertex_count;
  t->E = edges;
  t->En = edge_count;
  t->R = out_sorted;
  t->C = out_opt_components;
  t->index = 1;
  for (v = 0; v < t->Vn; ++v) {
    t->V[v].Vi = v;
    t->V[v].Ei = -1;
  }
  for (e = 0, v = -1; e < t->En; ++e) {
    if (t->E[e][0] == v) continue;
    v = t->E[e][0];
    t->V[v].Ei = e;
  }
  rc = 0;
  for (v = 0; v < t->Vn; ++v) {
    if (!t->V[v].index) {
      if (!TarjanConnect(t, v)) {
        free(t->S.p);
        free(t);
        return -1;
      }
    }
  }
  if (out_opt_components) {
    *out_opt_componentcount = t->Ci;
  }
  assert(t->Ri == vertex_count);
  free(t->S.p);
  free(t);
  return rc;
}
