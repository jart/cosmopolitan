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
#include "libc/bits/safemacros.internal.h"
#include "libc/fmt/fmt.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

struct ComboGroup {
  unsigned entry;
  unsigned i;
  unsigned n;
};

struct ComboProduct {
  unsigned n;
  struct ComboGroup groups[];
};

mallocesque testonly struct ComboProduct *testlib_setupcomboproduct(
    const struct TestFixture *start, const struct TestFixture *end) {
  unsigned i, j, entrycount;
  struct ComboProduct *product;
  entrycount = testlib_countfixtures(start, end);
  product = calloc(
      sizeof(struct ComboProduct) + entrycount * sizeof(struct ComboGroup), 1);
  for (j = i = 0; i < entrycount; ++i) {
    if (j && strcmp(start[product->groups[j - 1].entry].group,
                    start[i].group) == 0) {
      product->groups[j - 1].n++;
    } else {
      ++j;
      product->groups[j - 1].entry = i;
      product->groups[j - 1].n = 1;
    }
  }
  product->n = j;
  return product;
}

static testonly void testlib_describecombo(struct ComboProduct *product,
                                           const struct TestFixture *combos) {
  char *p = &g_fixturename[0];
  char *pe = p + sizeof(g_fixturename);
  for (unsigned i = 0; i < product->n && p < pe; ++i) {
    const char *sep = i ? ", " : "";
    const struct TestFixture *e =
        &combos[product->groups[i].entry + product->groups[i].i];
    p += max(0, snprintf(p, pe - p, "%s%s=%s", sep, e->group, e->name));
  }
}

static testonly void testlib_callcombos(struct ComboProduct *product,
                                        const struct TestFixture *combos,
                                        testfn_t *test_start,
                                        testfn_t *test_end) {
  for (;;) {
    testlib_describecombo(product, combos);
    for (unsigned i = 0; i < product->n; ++i) {
      combos[product->groups[i].entry + product->groups[i].i].fn();
    }
    for (unsigned i = product->n;; --i) {
      if (!i) return;
      if (++product->groups[i - 1].i < product->groups[i - 1].n) break;
      product->groups[i - 1].i = 0;
    }
    testlib_runtestcases(test_start, test_end, NULL);
  }
}

/**
 * Runs Cartesian product of COMBO() fixtures registered with linker.
 * @see ape/ape.lds
 * @see libc/testlib/testlib.h
 */
testonly void testlib_runcombos(testfn_t *test_start, testfn_t *test_end,
                                const struct TestFixture *combo_start,
                                const struct TestFixture *combo_end) {
  struct ComboProduct *product;
  product = testlib_setupcomboproduct(combo_start, combo_end);
  testlib_callcombos(product, combo_start, test_start, test_end);
  free(product);
}
