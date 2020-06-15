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

const ttypalette_t kXtermPalette = {
    [0][0] = {0, 0, 0, 0},        /* normal black:   \e[30m (or \e[38;5;0m) */
    [1][0] = {127, 127, 127, 8},  /* bright black:   \e[90m (or \e[38;5;8m) */
    [0][1] = {205, 0, 0, 1},      /* normal red:     \e[31m */
    [1][1] = {255, 0, 0, 9},      /* bright red:     \e[91m (or \e[1;31m) */
    [0][2] = {0, 205, 0, 2},      /* normal green:   \e[32m */
    [1][2] = {0, 255, 0, 10},     /* bright green:   \e[92m */
    [0][3] = {205, 205, 0, 3},    /* normal yellow:  \e[33m */
    [1][3] = {255, 255, 0, 11},   /* bright yellow:  \e[93m */
    [0][4] = {0, 0, 238, 4},      /* normal blue:    \e[34m */
    [1][4] = {92, 92, 255, 12},   /* bright blue:    \e[94m */
    [0][5] = {205, 0, 205, 5},    /* normal magenta: \e[35m */
    [1][5] = {255, 0, 255, 13},   /* bright magenta: \e[95m */
    [0][6] = {0, 205, 205, 6},    /* normal cyan:    \e[36m */
    [1][6] = {0, 255, 255, 14},   /* bright cyan:    \e[96m */
    [0][7] = {229, 229, 229, 7},  /* normal white:   \e[37m */
    [1][7] = {255, 255, 255, 15}, /* bright white:   \e[97m */
};
