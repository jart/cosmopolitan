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
#include "dsp/core/core.h"

/**
 * Computes Σ𝑥ᵢΣ𝑥ⱼΣ𝑥ₖΣ𝑥ₗΣ𝑥ₘΣ𝑥ₙ(δ₁𝑥ᵢ+δ₂𝑥ⱼ+δ₃𝑥ₖ+δ₄𝑥ₗ+δ₅𝑥ₘ+δ₆𝑥ₙ)² over 𝐿..𝐻
 *
 *   “As soon as an Analytical Engine exists, it will necessarily
 *    guide the future course of the science. Whenever any result
 *    is sought by its aid, the question will then arise — by what
 *    course of calculation can these results be arrived at by the
 *    machine in the shortest time?
 *
 *                    — Charles Babbage (Life of a Philosopher, 1864)
 *
 * @see itu.int/rec/R-REC-BT.601/
 */
double DifferSumSq(const double D[static 6], double L, double H) {
  double T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T2, T20, T21, T22,
      T23, T24, T25, T26, T27, T3, T4, T5, T6, T7, T8, T9;
  T2 = H * H, T3 = (H * H * H), T4 = (H * H * H * H), T5 = (H * H * H * H * H),
  T6 = (H * H * H * H * H * H), T7 = -10 * H, T8 = (H * H * H * H * H * H * H),
  T9 = (L * L * L * L * L * L * L * L), T10 = (L * L * L * L * L * L * L),
  T11 = (L * L * L * L * L * L), T12 = (L * L * L * L * L), T13 = -45 * T2,
  T14 = (L * L * L * L), T15 = 180 * T3, T16 = 120 * T2, T17 = (L * L * L),
  T18 = L * L, T19 = 18 * T2, T20 = (H * H * H * H * H * H * H * H);
  T21 = 45 * T4;
  T22 = 3 * T9 + (-12 * H - 18) * T10 + (12 * T2 + 54 * H + 45) * T11 +
        (12 * T3 - T19 - 90 * H - 60) * T12 +
        (-30 * T4 - 90 * T3 + T13 + 60 * H + 45) * T14 +
        (12 * T5 + 90 * T4 + T15 + T16 - 18) * T17 +
        (12 * T6 + 18 * T5 - T21 - 120 * T3 - 90 * T2 - 18 * H + 3) * T18 +
        (-12 * T8 - 54 * T6 - 90 * T5 - 60 * T4 + T19 + 6 * H) * L + 3 * T20 +
        18 * T8 + 45 * T6 + 60 * T5 + T21 + 18 * T3 + 3 * T2;
  T23 =
      2 * T9 + (T7 - 13) * T10 + (20 * T2 + 55 * H + 36) * T11 +
      (-22 * T3 - 93 * T2 - 126 * H - 55) * T12 +
      (20 * T4 + 95 * T3 + 180 * T2 + 155 * H + 50) * T14 +
      (-22 * T5 - 95 * T4 - T15 - 190 * T2 - 110 * H - 27) * T17 +
      (20 * T6 + 93 * T5 + 180 * T4 + 190 * T3 + T16 + 45 * H + 8) * T18 +
      (-10 * T8 - 55 * T6 - 126 * T5 - 155 * T4 - 110 * T3 + T13 + T7 - 1) * L +
      2 * T20 + 13 * T8 + 36 * T6 + 55 * T5 + 50 * T4 + 27 * T3 + 8 * T2 + H;
  T24 = T22 * D[3], T25 = T22 * D[2], T26 = T22 * D[1], T27 = T22 * D[0];
  return (T23 * D[5] * D[5] + (T22 * D[4] + T24 + T25 + T26 + T27) * D[5] +
          T23 * D[4] * D[4] + (T24 + T25 + T26 + T27) * D[4] +
          T23 * D[3] * D[3] + (T25 + T26 + T27) * D[3] + T23 * D[2] * D[2] +
          (T26 + T27) * D[2] + T23 * D[1] * D[1] + T22 * D[0] * D[1] +
          T23 * D[0] * D[0]) /
         6;
}
