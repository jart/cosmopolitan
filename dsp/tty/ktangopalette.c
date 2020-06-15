#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "dsp/tty/quant.h"

const ttypalette_t kTangoPalette = {
    [0][0] = {0x2E, 0x34, 0x36, 0}, /* aluminium1 */
    [1][0] = {0x55, 0x57, 0x53, 8},
    [0][1] = {0xCC, 0x00, 0x00, 1}, /* scarietred */
    [1][1] = {0xEF, 0x29, 0x29, 9},
    [0][2] = {0x4E, 0x9A, 0x06, 2}, /* chameleon */
    [1][2] = {0x8A, 0xE2, 0x34, 10},
    [0][3] = {0xC4, 0xA0, 0x00, 3}, /* butter */
    [1][3] = {0xFC, 0xE9, 0x4F, 11},
    [0][4] = {0x34, 0x65, 0xA4, 4}, /* skyblue */
    [1][4] = {0x72, 0x9F, 0xCF, 12},
    [0][5] = {0x75, 0x50, 0x7B, 5}, /* plum */
    [1][5] = {0xAD, 0x7F, 0xA8, 13},
    [0][6] = {0x06, 0x98, 0x9A, 6}, /* cyan */
    [1][6] = {0x34, 0xE2, 0xE2, 14},
    [0][7] = {0xD3, 0xD7, 0xCF, 7}, /* aluminium2 */
    [1][7] = {0xEE, 0xEE, 0xEC, 15},
};
