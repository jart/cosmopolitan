/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/alg/alg.h"
#include "libc/assert.h"
#include "libc/bits/safemacros.h"
#include "libc/limits.h"
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

struct Vertex {
  uint32_t Vi;
  uint32_t Ei;
  uint32_t index;
  uint32_t lowlink;
  bool onstack;
  bool selfreferential;
};

struct TarjanStack {
  size_t i;
  size_t n;
  uint32_t *p;
};

struct Tarjan {
  uint32_t Vn;
  uint32_t En;
  struct Vertex *V;
  const uint32_t (*E)[2];
  uint32_t *R;
  uint32_t *C;
  uint32_t Ci;
  uint32_t Ri;
  uint32_t index;
  struct TarjanStack S;
};

static uint32_t TarjanPush(struct TarjanStack *st, uint32_t Vi) {
  if (st->i < st->n || grow(&st->p, &st->n, sizeof(uint32_t), 0)) {
    return (st->p[st->i++] = Vi);
  } else {
    return -1;
  }
}

static uint32_t TarjanPop(struct TarjanStack *st) {
  assert(st->i != 0);
  return st->p[--st->i];
}

static int TarjanConnect(struct Tarjan **tj, uint32_t Vi) {
  struct Vertex *v = &(*tj)->V[Vi];
  v->index = (*tj)->index;
  v->lowlink = (*tj)->index;
  v->onstack = true;
  (*tj)->index++;
  if (TarjanPush(&(*tj)->S, Vi) == -1) return -1;
  uint32_t fs = (*tj)->V[Vi].Ei;
  if (fs != -1) {
    for (uint32_t Ei = fs; Ei < (*tj)->En && Vi == (*tj)->E[Ei][0]; ++Ei) {
      struct Vertex *w = &(*tj)->V[(*tj)->E[Ei][1]];
      if (!w->index) {
        if (TarjanConnect(tj, w->Vi) == -1) return -1;
        v->lowlink = min(v->lowlink, w->lowlink);
      } else if (w->onstack) {
        v->lowlink = min(v->lowlink, w->index);
      }
      if (w == v) {
        w->selfreferential = true;
      }
    }
  }
  if (v->lowlink == v->index) {
    struct Vertex *w;
    do {
      w = &(*tj)->V[TarjanPop(&(*tj)->S)];
      w->onstack = false;
      (*tj)->R[(*tj)->Ri++] = w->Vi;
    } while (w != v);
    if ((*tj)->C) (*tj)->C[(*tj)->Ci++] = (*tj)->Ri;
  }
  return 0;
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
int tarjan(uint32_t vertex_count, const uint32_t (*edges)[2],
           uint32_t edge_count, uint32_t out_sorted[],
           uint32_t out_opt_components[], uint32_t *out_opt_componentcount) {
  assert(edge_count <= INT_MAX);
  assert(vertex_count <= INT_MAX);
  for (unsigned i = 0; i < edge_count; ++i) {
    if (i) assert(edges[i - 1][0] <= edges[i][0]);
    assert(edges[i][0] < vertex_count);
    assert(edges[i][1] < vertex_count);
  }
  int rc;
  struct Tarjan *tj;
  if ((tj = calloc(1, (sizeof(struct Tarjan) +
                       sizeof(struct Vertex) * vertex_count)))) {
    tj->V = (struct Vertex *)((char *)tj + sizeof(struct Tarjan));
    tj->Vn = vertex_count;
    tj->E = edges;
    tj->En = edge_count;
    tj->R = out_sorted;
    tj->C = out_opt_components;
    tj->index = 1;
    uint32_t Vi, Ei;
    for (Vi = 0; Vi < tj->Vn; ++Vi) {
      tj->V[Vi].Vi = Vi;
      tj->V[Vi].Ei = -1u;
    }
    for (Ei = 0, Vi = -1u; Ei < tj->En; ++Ei) {
      if (tj->E[Ei][0] == Vi) continue;
      Vi = tj->E[Ei][0];
      tj->V[Vi].Ei = Ei;
    }
    rc = 0;
    for (Vi = 0; Vi < tj->Vn; ++Vi) {
      if (!tj->V[Vi].index) {
        if ((rc = TarjanConnect(&tj, Vi)) == -1) {
          break;
        }
      }
    }
    free(tj->S.p);
    assert(tj->Ri == vertex_count);
    if (out_opt_components) *out_opt_componentcount = tj->Ci;
  } else {
    rc = -1;
  }
  free(tj);
  return rc;
}
