/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/python/Include/grammar.h"
#include "third_party/python/Include/metagrammar.h"
#include "third_party/python/Include/pgen.h"
#include "third_party/python/Include/pgenheaders.h"

static arc arcs_0_0[3] = {
    {2, 0},
    {3, 0},
    {4, 1},
};
static arc arcs_0_1[1] = {
    {0, 1},
};
static state states_0[2] = {
    {.s_narcs=3, .s_arc=arcs_0_0},
    {.s_narcs=1, .s_arc=arcs_0_1},
};
static arc arcs_1_0[1] = {
    {5, 1},
};
static arc arcs_1_1[1] = {
    {6, 2},
};
static arc arcs_1_2[1] = {
    {7, 3},
};
static arc arcs_1_3[1] = {
    {3, 4},
};
static arc arcs_1_4[1] = {
    {0, 4},
};
static state states_1[5] = {
    {.s_narcs=1, .s_arc=arcs_1_0},
    {.s_narcs=1, .s_arc=arcs_1_1},
    {.s_narcs=1, .s_arc=arcs_1_2},
    {.s_narcs=1, .s_arc=arcs_1_3},
    {.s_narcs=1, .s_arc=arcs_1_4},
};
static arc arcs_2_0[1] = {
    {8, 1},
};
static arc arcs_2_1[2] = {
    {9, 0},
    {0, 1},
};
static state states_2[2] = {
    {.s_narcs=1, .s_arc=arcs_2_0},
    {.s_narcs=2, .s_arc=arcs_2_1},
};
static arc arcs_3_0[1] = {
    {10, 1},
};
static arc arcs_3_1[2] = {
    {10, 1},
    {0, 1},
};
static state states_3[2] = {
    {.s_narcs=1, .s_arc=arcs_3_0},
    {.s_narcs=2, .s_arc=arcs_3_1},
};
static arc arcs_4_0[2] = {
    {11, 1},
    {13, 2},
};
static arc arcs_4_1[1] = {
    {7, 3},
};
static arc arcs_4_2[3] = {
    {14, 4},
    {15, 4},
    {0, 2},
};
static arc arcs_4_3[1] = {
    {12, 4},
};
static arc arcs_4_4[1] = {
    {0, 4},
};
static state states_4[5] = {
    {.s_narcs=2, .s_arc=arcs_4_0},
    {.s_narcs=1, .s_arc=arcs_4_1},
    {.s_narcs=3, .s_arc=arcs_4_2},
    {.s_narcs=1, .s_arc=arcs_4_3},
    {.s_narcs=1, .s_arc=arcs_4_4},
};
static arc arcs_5_0[3] = {
    {5, 1},
    {16, 1},
    {17, 2},
};
static arc arcs_5_1[1] = {
    {0, 1},
};
static arc arcs_5_2[1] = {
    {7, 3},
};
static arc arcs_5_3[1] = {
    {18, 1},
};
static state states_5[4] = {
    {.s_narcs=3, .s_arc=arcs_5_0},
    {.s_narcs=1, .s_arc=arcs_5_1},
    {.s_narcs=1, .s_arc=arcs_5_2},
    {.s_narcs=1, .s_arc=arcs_5_3},
};
static dfa dfas[6] = {
    {256, "MSTART", 0, 2, states_0,
     "\070\000\000"},
    {257, "RULE", 0, 5, states_1,
     "\040\000\000"},
    {258, "RHS", 0, 2, states_2,
     "\040\010\003"},
    {259, "ALT", 0, 2, states_3,
     "\040\010\003"},
    {260, "ITEM", 0, 5, states_4,
     "\040\010\003"},
    {261, "ATOM", 0, 4, states_5,
     "\040\000\003"},
};
static label labels[19] = {
    {0, "EMPTY"},
    {256, 0},
    {257, 0},
    {4, 0},
    {0, 0},
    {1, 0},
    {11, 0},
    {258, 0},
    {259, 0},
    {18, 0},
    {260, 0},
    {9, 0},
    {10, 0},
    {261, 0},
    {16, 0},
    {14, 0},
    {3, 0},
    {7, 0},
    {8, 0},
};
static grammar _PyParser_Grammarz = {
    6,
    dfas,
    {19, labels},
    256
};

grammar *
meta_grammar(void)
{
    return &_PyParser_Grammarz;
}

grammar *
Py_meta_grammar(void)
{
  return meta_grammar();
}
