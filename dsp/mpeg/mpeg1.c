/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:4;tab-width:4;coding:utf-8   -*-│
│ vi: set et ft=c ts=4 sw=4 fenc=utf-8                                     :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│  PL_MPEG - MPEG1 Video decoder, MP2 Audio decoder, MPEG-PS demuxer           │
│  Dominic Szablewski - https://phoboslab.org                                  │
│                                                                              │
│  The MIT License(MIT)                                                        │
│  Copyright(c) 2019 Dominic Szablewski                                        │
│                                                                              │
│  Permission is hereby granted, free of charge, to any person obtaining       │
│  a copy of this software and associated documentation files(the              │
│  "Software"), to deal in the Software without restriction, including         │
│  without limitation the rights to use, copy, modify, merge, publish,         │
│  distribute, sublicense, and / or sell copies of the Software, and to        │
│  permit persons to whom the Software is furnished to do so, subject to       │
│  the following conditions:                                                   │
│                                                                              │
│    The above copyright notice and this permission notice shall be            │
│    included in all copies or substantial portions of the Software.           │
│                                                                              │
│  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,             │
│  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF          │
│  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND                       │
│  NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE       │
│  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN             │
│  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN           │
│  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE            │
│  SOFTWARE.                                                                   │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "dsp/mpeg/blockset.h"
#include "dsp/mpeg/buffer.h"
#include "dsp/mpeg/idct.h"
#include "dsp/mpeg/mpeg.h"
#include "dsp/mpeg/video.h"
#include "libc/calls/struct/timespec.h"
#include "libc/fmt/conv.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/time/time.h"
#include "libc/x/x.h"

asm(".ident\t\"\\n\\n\
PL_MPEG (MIT License)\\n\
Copyright(c) 2019 Dominic Szablewski\\n\
https://phoboslab.org\"");
asm(".include \"libc/disclaimer.inc\"");

// -----------------------------------------------------------------------------
// plm_video implementation

// Inspired by Java MPEG-1 Video Decoder and Player by Zoltan Korandi
// https://sourceforge.net/projects/javampeg1video/

#define GETCONST(ARRAY, DEFAULT)

static const int PLM_VIDEO_PICTURE_TYPE_INTRA = 1;
static const int PLM_VIDEO_PICTURE_TYPE_PREDICTIVE = 2;
static const int PLM_VIDEO_PICTURE_TYPE_B = 3;

static const int PLM_START_SEQUENCE = 0xB3;
static const int PLM_START_SLICE_FIRST = 0x01;
static const int PLM_START_SLICE_LAST = 0xAF;
static const int PLM_START_PICTURE = 0x00;
static const int PLM_START_EXTENSION = 0xB5;
static const int PLM_START_USER_DATA = 0xB2;

static const float PLM_VIDEO_PIXEL_ASPECT_RATIO[] = {
    1.0000, /* square pixels */
    0.6735, /* 3:4? */
    0.7031, /* MPEG-1 / MPEG-2 video encoding divergence? */
    0.7615, 0.8055, 0.8437, 0.8935, 0.9157, 0.9815,
    1.0255, 1.0695, 1.0950, 1.1575, 1.2051,
};

static const float PLM_VIDEO_PICTURE_RATE[] = {
    23.976, /* NTSC-Film */
    24.000, /* NTSC-Film (enriched for foreign nations) */
    25.000, /* PAL (Britain, Africa, China, etc.) */
    29.970, /* NTSC */
    30.000, /* NTSC (enriched for foreign nations) */
    50.000, /* PAL? */
    59.940, /* NTSC-Wow */
    60.000  /* NTSC-Wow (enriched for foreign nations) */
};

static const uint8_t PLM_VIDEO_ZIG_ZAG[] = /* clang-format off */ {
     0,  1,  8, 16,  9,  2,  3, 10,
	17, 24, 32, 25, 18, 11,  4,  5,
    12, 19, 26, 33, 40, 48, 41, 34,
	27, 20, 13,  6,  7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
	29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
	53, 60, 61, 54, 47, 55, 62, 63,
} /* clang-format on */;

static const uint8_t PLM_VIDEO_INTRAQUANT_MATRIX[] = /* clang-format off */ {
     8, 16, 19, 22, 26, 27, 29, 34,
	16, 16, 22, 24, 27, 29, 34, 37,
    19, 22, 26, 27, 29, 34, 34, 38,
	22, 22, 26, 27, 29, 34, 37, 40,
    22, 26, 27, 29, 32, 35, 40, 48,
	26, 27, 29, 32, 35, 40, 48, 58,
    26, 27, 29, 34, 38, 46, 56, 69,
	27, 29, 35, 38, 46, 56, 69, 83,
} /* clang-format on */;

static const uint8_t PLM_VIDEO_NONINTRAQUANT_MATRIX[] = /* clang-format off */ {
    16, 16, 16, 16, 16, 16, 16, 16,
	16, 16, 16, 16, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 16, 16, 16,
	16, 16, 16, 16, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 16, 16, 16,
	16, 16, 16, 16, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 16, 16, 16,
	16, 16, 16, 16, 16, 16, 16, 16,
} /* clang-format on */;

static const uint8_t PLM_VIDEO_PREMULTIPLIER_MATRIX[] = /* clang-format off */ {
    32, 44, 42, 38, 32, 25, 17,  9,
	44, 62, 58, 52, 44, 35, 24, 12,
    42, 58, 55, 49, 42, 33, 23, 12,
	38, 52, 49, 44, 38, 30, 20, 10,
    32, 44, 42, 38, 32, 25, 17,  9,
	25, 35, 33, 30, 25, 20, 14,  7,
    17, 24, 23, 20, 17, 14,  9,  5,
	 9, 12, 12, 10,  9,  7,  5,  2,
} /* clang-format on */;

static const plm_vlc_t PLM_VIDEO_MACROBLOCK_ADDRESS_INCREMENT[] = {
    {1 << 1, 0},  {0, 1},        //   0: x
    {2 << 1, 0},  {3 << 1, 0},   //   1: 0x
    {4 << 1, 0},  {5 << 1, 0},   //   2: 00x
    {0, 3},       {0, 2},        //   3: 01x
    {6 << 1, 0},  {7 << 1, 0},   //   4: 000x
    {0, 5},       {0, 4},        //   5: 001x
    {8 << 1, 0},  {9 << 1, 0},   //   6: 0000x
    {0, 7},       {0, 6},        //   7: 0001x
    {10 << 1, 0}, {11 << 1, 0},  //   8: 0000 0x
    {12 << 1, 0}, {13 << 1, 0},  //   9: 0000 1x
    {14 << 1, 0}, {15 << 1, 0},  //  10: 0000 00x
    {16 << 1, 0}, {17 << 1, 0},  //  11: 0000 01x
    {18 << 1, 0}, {19 << 1, 0},  //  12: 0000 10x
    {0, 9},       {0, 8},        //  13: 0000 11x
    {-1, 0},      {20 << 1, 0},  //  14: 0000 000x
    {-1, 0},      {21 << 1, 0},  //  15: 0000 001x
    {22 << 1, 0}, {23 << 1, 0},  //  16: 0000 010x
    {0, 15},      {0, 14},       //  17: 0000 011x
    {0, 13},      {0, 12},       //  18: 0000 100x
    {0, 11},      {0, 10},       //  19: 0000 101x
    {24 << 1, 0}, {25 << 1, 0},  //  20: 0000 0001x
    {26 << 1, 0}, {27 << 1, 0},  //  21: 0000 0011x
    {28 << 1, 0}, {29 << 1, 0},  //  22: 0000 0100x
    {30 << 1, 0}, {31 << 1, 0},  //  23: 0000 0101x
    {32 << 1, 0}, {-1, 0},       //  24: 0000 0001 0x
    {-1, 0},      {33 << 1, 0},  //  25: 0000 0001 1x
    {34 << 1, 0}, {35 << 1, 0},  //  26: 0000 0011 0x
    {36 << 1, 0}, {37 << 1, 0},  //  27: 0000 0011 1x
    {38 << 1, 0}, {39 << 1, 0},  //  28: 0000 0100 0x
    {0, 21},      {0, 20},       //  29: 0000 0100 1x
    {0, 19},      {0, 18},       //  30: 0000 0101 0x
    {0, 17},      {0, 16},       //  31: 0000 0101 1x
    {0, 35},      {-1, 0},       //  32: 0000 0001 00x
    {-1, 0},      {0, 34},       //  33: 0000 0001 11x
    {0, 33},      {0, 32},       //  34: 0000 0011 00x
    {0, 31},      {0, 30},       //  35: 0000 0011 01x
    {0, 29},      {0, 28},       //  36: 0000 0011 10x
    {0, 27},      {0, 26},       //  37: 0000 0011 11x
    {0, 25},      {0, 24},       //  38: 0000 0100 00x
    {0, 23},      {0, 22},       //  39: 0000 0100 01x
};

static const plm_vlc_t PLM_VIDEO_MACROBLOCK_TYPE_INTRA[] = {
    {1 << 1, 0},
    {0, 0x01},  //   0: x
    {-1, 0},
    {0, 0x11},  //   1: 0x
};

static const plm_vlc_t PLM_VIDEO_MACROBLOCK_TYPE_PREDICTIVE[] = {
    {1 << 1, 0}, {0, 0x0a},    //   0: x
    {2 << 1, 0}, {0, 0x02},    //   1: 0x
    {3 << 1, 0}, {0, 0x08},    //   2: 00x
    {4 << 1, 0}, {5 << 1, 0},  //   3: 000x
    {6 << 1, 0}, {0, 0x12},    //   4: 0000x
    {0, 0x1a},   {0, 0x01},    //   5: 0001x
    {-1, 0},     {0, 0x11},    //   6: 0000 0x
};

static const plm_vlc_t PLM_VIDEO_MACROBLOCK_TYPE_B[] = {
    {1 << 1, 0}, {2 << 1, 0},   //   0: x
    {3 << 1, 0}, {4 << 1, 0},   //   1: 0x
    {0, 0x0c},   {0, 0x0e},     //   2: 1x
    {5 << 1, 0}, {6 << 1, 0},   //   3: 00x
    {0, 0x04},   {0, 0x06},     //   4: 01x
    {7 << 1, 0}, {8 << 1, 0},   //   5: 000x
    {0, 0x08},   {0, 0x0a},     //   6: 001x
    {9 << 1, 0}, {10 << 1, 0},  //   7: 0000x
    {0, 0x1e},   {0, 0x01},     //   8: 0001x
    {-1, 0},     {0, 0x11},     //   9: 0000 0x
    {0, 0x16},   {0, 0x1a},     //  10: 0000 1x
};

static const plm_vlc_t PLM_VIDEO_CODE_BLOCK_PATTERN[] = {
    {1 << 1, 0},  {2 << 1, 0},   //   0: x
    {3 << 1, 0},  {4 << 1, 0},   //   1: 0x
    {5 << 1, 0},  {6 << 1, 0},   //   2: 1x
    {7 << 1, 0},  {8 << 1, 0},   //   3: 00x
    {9 << 1, 0},  {10 << 1, 0},  //   4: 01x
    {11 << 1, 0}, {12 << 1, 0},  //   5: 10x
    {13 << 1, 0}, {0, 60},       //   6: 11x
    {14 << 1, 0}, {15 << 1, 0},  //   7: 000x
    {16 << 1, 0}, {17 << 1, 0},  //   8: 001x
    {18 << 1, 0}, {19 << 1, 0},  //   9: 010x
    {20 << 1, 0}, {21 << 1, 0},  //  10: 011x
    {22 << 1, 0}, {23 << 1, 0},  //  11: 100x
    {0, 32},      {0, 16},       //  12: 101x
    {0, 8},       {0, 4},        //  13: 110x
    {24 << 1, 0}, {25 << 1, 0},  //  14: 0000x
    {26 << 1, 0}, {27 << 1, 0},  //  15: 0001x
    {28 << 1, 0}, {29 << 1, 0},  //  16: 0010x
    {30 << 1, 0}, {31 << 1, 0},  //  17: 0011x
    {0, 62},      {0, 2},        //  18: 0100x
    {0, 61},      {0, 1},        //  19: 0101x
    {0, 56},      {0, 52},       //  20: 0110x
    {0, 44},      {0, 28},       //  21: 0111x
    {0, 40},      {0, 20},       //  22: 1000x
    {0, 48},      {0, 12},       //  23: 1001x
    {32 << 1, 0}, {33 << 1, 0},  //  24: 0000 0x
    {34 << 1, 0}, {35 << 1, 0},  //  25: 0000 1x
    {36 << 1, 0}, {37 << 1, 0},  //  26: 0001 0x
    {38 << 1, 0}, {39 << 1, 0},  //  27: 0001 1x
    {40 << 1, 0}, {41 << 1, 0},  //  28: 0010 0x
    {42 << 1, 0}, {43 << 1, 0},  //  29: 0010 1x
    {0, 63},      {0, 3},        //  30: 0011 0x
    {0, 36},      {0, 24},       //  31: 0011 1x
    {44 << 1, 0}, {45 << 1, 0},  //  32: 0000 00x
    {46 << 1, 0}, {47 << 1, 0},  //  33: 0000 01x
    {48 << 1, 0}, {49 << 1, 0},  //  34: 0000 10x
    {50 << 1, 0}, {51 << 1, 0},  //  35: 0000 11x
    {52 << 1, 0}, {53 << 1, 0},  //  36: 0001 00x
    {54 << 1, 0}, {55 << 1, 0},  //  37: 0001 01x
    {56 << 1, 0}, {57 << 1, 0},  //  38: 0001 10x
    {58 << 1, 0}, {59 << 1, 0},  //  39: 0001 11x
    {0, 34},      {0, 18},       //  40: 0010 00x
    {0, 10},      {0, 6},        //  41: 0010 01x
    {0, 33},      {0, 17},       //  42: 0010 10x
    {0, 9},       {0, 5},        //  43: 0010 11x
    {-1, 0},      {60 << 1, 0},  //  44: 0000 000x
    {61 << 1, 0}, {62 << 1, 0},  //  45: 0000 001x
    {0, 58},      {0, 54},       //  46: 0000 010x
    {0, 46},      {0, 30},       //  47: 0000 011x
    {0, 57},      {0, 53},       //  48: 0000 100x
    {0, 45},      {0, 29},       //  49: 0000 101x
    {0, 38},      {0, 26},       //  50: 0000 110x
    {0, 37},      {0, 25},       //  51: 0000 111x
    {0, 43},      {0, 23},       //  52: 0001 000x
    {0, 51},      {0, 15},       //  53: 0001 001x
    {0, 42},      {0, 22},       //  54: 0001 010x
    {0, 50},      {0, 14},       //  55: 0001 011x
    {0, 41},      {0, 21},       //  56: 0001 100x
    {0, 49},      {0, 13},       //  57: 0001 101x
    {0, 35},      {0, 19},       //  58: 0001 110x
    {0, 11},      {0, 7},        //  59: 0001 111x
    {0, 39},      {0, 27},       //  60: 0000 0001x
    {0, 59},      {0, 55},       //  61: 0000 0010x
    {0, 47},      {0, 31},       //  62: 0000 0011x
};

static const plm_vlc_t PLM_VIDEO_MOTION[] = {
    {1 << 1, 0},  {0, 0},        //   0: x
    {2 << 1, 0},  {3 << 1, 0},   //   1: 0x
    {4 << 1, 0},  {5 << 1, 0},   //   2: 00x
    {0, 1},       {0, -1},       //   3: 01x
    {6 << 1, 0},  {7 << 1, 0},   //   4: 000x
    {0, 2},       {0, -2},       //   5: 001x
    {8 << 1, 0},  {9 << 1, 0},   //   6: 0000x
    {0, 3},       {0, -3},       //   7: 0001x
    {10 << 1, 0}, {11 << 1, 0},  //   8: 0000 0x
    {12 << 1, 0}, {13 << 1, 0},  //   9: 0000 1x
    {-1, 0},      {14 << 1, 0},  //  10: 0000 00x
    {15 << 1, 0}, {16 << 1, 0},  //  11: 0000 01x
    {17 << 1, 0}, {18 << 1, 0},  //  12: 0000 10x
    {0, 4},       {0, -4},       //  13: 0000 11x
    {-1, 0},      {19 << 1, 0},  //  14: 0000 001x
    {20 << 1, 0}, {21 << 1, 0},  //  15: 0000 010x
    {0, 7},       {0, -7},       //  16: 0000 011x
    {0, 6},       {0, -6},       //  17: 0000 100x
    {0, 5},       {0, -5},       //  18: 0000 101x
    {22 << 1, 0}, {23 << 1, 0},  //  19: 0000 0011x
    {24 << 1, 0}, {25 << 1, 0},  //  20: 0000 0100x
    {26 << 1, 0}, {27 << 1, 0},  //  21: 0000 0101x
    {28 << 1, 0}, {29 << 1, 0},  //  22: 0000 0011 0x
    {30 << 1, 0}, {31 << 1, 0},  //  23: 0000 0011 1x
    {32 << 1, 0}, {33 << 1, 0},  //  24: 0000 0100 0x
    {0, 10},      {0, -10},      //  25: 0000 0100 1x
    {0, 9},       {0, -9},       //  26: 0000 0101 0x
    {0, 8},       {0, -8},       //  27: 0000 0101 1x
    {0, 16},      {0, -16},      //  28: 0000 0011 00x
    {0, 15},      {0, -15},      //  29: 0000 0011 01x
    {0, 14},      {0, -14},      //  30: 0000 0011 10x
    {0, 13},      {0, -13},      //  31: 0000 0011 11x
    {0, 12},      {0, -12},      //  32: 0000 0100 00x
    {0, 11},      {0, -11},      //  33: 0000 0100 01x
};

static const plm_vlc_t PLM_VIDEO_DCT_SIZE_LUMINANCE[] = {
    {1 << 1, 0}, {2 << 1, 0},  //   0: x
    {0, 1},      {0, 2},       //   1: 0x
    {3 << 1, 0}, {4 << 1, 0},  //   2: 1x
    {0, 0},      {0, 3},       //   3: 10x
    {0, 4},      {5 << 1, 0},  //   4: 11x
    {0, 5},      {6 << 1, 0},  //   5: 111x
    {0, 6},      {7 << 1, 0},  //   6: 1111x
    {0, 7},      {8 << 1, 0},  //   7: 1111 1x
    {0, 8},      {-1, 0},      //   8: 1111 11x
};

static const plm_vlc_t PLM_VIDEO_DCT_SIZE_CHROMINANCE[] = {
    {1 << 1, 0}, {2 << 1, 0},  //   0: x
    {0, 0},      {0, 1},       //   1: 0x
    {0, 2},      {3 << 1, 0},  //   2: 1x
    {0, 3},      {4 << 1, 0},  //   3: 11x
    {0, 4},      {5 << 1, 0},  //   4: 111x
    {0, 5},      {6 << 1, 0},  //   5: 1111x
    {0, 6},      {7 << 1, 0},  //   6: 1111 1x
    {0, 7},      {8 << 1, 0},  //   7: 1111 11x
    {0, 8},      {-1, 0},      //   8: 1111 111x
};

//  dct_coeff bitmap:
//    0xff00  run
//    0x00ff  level

//  Decoded values are unsigned. Sign bit follows in the stream.

static const plm_vlc_uint_t PLM_VIDEO_DCT_COEFF[] = {
    {1 << 1, 0},   {0, 0x0001},    //   0: x
    {2 << 1, 0},   {3 << 1, 0},    //   1: 0x
    {4 << 1, 0},   {5 << 1, 0},    //   2: 00x
    {6 << 1, 0},   {0, 0x0101},    //   3: 01x
    {7 << 1, 0},   {8 << 1, 0},    //   4: 000x
    {9 << 1, 0},   {10 << 1, 0},   //   5: 001x
    {0, 0x0002},   {0, 0x0201},    //   6: 010x
    {11 << 1, 0},  {12 << 1, 0},   //   7: 0000x
    {13 << 1, 0},  {14 << 1, 0},   //   8: 0001x
    {15 << 1, 0},  {0, 0x0003},    //   9: 0010x
    {0, 0x0401},   {0, 0x0301},    //  10: 0011x
    {16 << 1, 0},  {0, 0xffff},    //  11: 0000 0x
    {17 << 1, 0},  {18 << 1, 0},   //  12: 0000 1x
    {0, 0x0701},   {0, 0x0601},    //  13: 0001 0x
    {0, 0x0102},   {0, 0x0501},    //  14: 0001 1x
    {19 << 1, 0},  {20 << 1, 0},   //  15: 0010 0x
    {21 << 1, 0},  {22 << 1, 0},   //  16: 0000 00x
    {0, 0x0202},   {0, 0x0901},    //  17: 0000 10x
    {0, 0x0004},   {0, 0x0801},    //  18: 0000 11x
    {23 << 1, 0},  {24 << 1, 0},   //  19: 0010 00x
    {25 << 1, 0},  {26 << 1, 0},   //  20: 0010 01x
    {27 << 1, 0},  {28 << 1, 0},   //  21: 0000 000x
    {29 << 1, 0},  {30 << 1, 0},   //  22: 0000 001x
    {0, 0x0d01},   {0, 0x0006},    //  23: 0010 000x
    {0, 0x0c01},   {0, 0x0b01},    //  24: 0010 001x
    {0, 0x0302},   {0, 0x0103},    //  25: 0010 010x
    {0, 0x0005},   {0, 0x0a01},    //  26: 0010 011x
    {31 << 1, 0},  {32 << 1, 0},   //  27: 0000 0000x
    {33 << 1, 0},  {34 << 1, 0},   //  28: 0000 0001x
    {35 << 1, 0},  {36 << 1, 0},   //  29: 0000 0010x
    {37 << 1, 0},  {38 << 1, 0},   //  30: 0000 0011x
    {39 << 1, 0},  {40 << 1, 0},   //  31: 0000 0000 0x
    {41 << 1, 0},  {42 << 1, 0},   //  32: 0000 0000 1x
    {43 << 1, 0},  {44 << 1, 0},   //  33: 0000 0001 0x
    {45 << 1, 0},  {46 << 1, 0},   //  34: 0000 0001 1x
    {0, 0x1001},   {0, 0x0502},    //  35: 0000 0010 0x
    {0, 0x0007},   {0, 0x0203},    //  36: 0000 0010 1x
    {0, 0x0104},   {0, 0x0f01},    //  37: 0000 0011 0x
    {0, 0x0e01},   {0, 0x0402},    //  38: 0000 0011 1x
    {47 << 1, 0},  {48 << 1, 0},   //  39: 0000 0000 00x
    {49 << 1, 0},  {50 << 1, 0},   //  40: 0000 0000 01x
    {51 << 1, 0},  {52 << 1, 0},   //  41: 0000 0000 10x
    {53 << 1, 0},  {54 << 1, 0},   //  42: 0000 0000 11x
    {55 << 1, 0},  {56 << 1, 0},   //  43: 0000 0001 00x
    {57 << 1, 0},  {58 << 1, 0},   //  44: 0000 0001 01x
    {59 << 1, 0},  {60 << 1, 0},   //  45: 0000 0001 10x
    {61 << 1, 0},  {62 << 1, 0},   //  46: 0000 0001 11x
    {-1, 0},       {63 << 1, 0},   //  47: 0000 0000 000x
    {64 << 1, 0},  {65 << 1, 0},   //  48: 0000 0000 001x
    {66 << 1, 0},  {67 << 1, 0},   //  49: 0000 0000 010x
    {68 << 1, 0},  {69 << 1, 0},   //  50: 0000 0000 011x
    {70 << 1, 0},  {71 << 1, 0},   //  51: 0000 0000 100x
    {72 << 1, 0},  {73 << 1, 0},   //  52: 0000 0000 101x
    {74 << 1, 0},  {75 << 1, 0},   //  53: 0000 0000 110x
    {76 << 1, 0},  {77 << 1, 0},   //  54: 0000 0000 111x
    {0, 0x000b},   {0, 0x0802},    //  55: 0000 0001 000x
    {0, 0x0403},   {0, 0x000a},    //  56: 0000 0001 001x
    {0, 0x0204},   {0, 0x0702},    //  57: 0000 0001 010x
    {0, 0x1501},   {0, 0x1401},    //  58: 0000 0001 011x
    {0, 0x0009},   {0, 0x1301},    //  59: 0000 0001 100x
    {0, 0x1201},   {0, 0x0105},    //  60: 0000 0001 101x
    {0, 0x0303},   {0, 0x0008},    //  61: 0000 0001 110x
    {0, 0x0602},   {0, 0x1101},    //  62: 0000 0001 111x
    {78 << 1, 0},  {79 << 1, 0},   //  63: 0000 0000 0001x
    {80 << 1, 0},  {81 << 1, 0},   //  64: 0000 0000 0010x
    {82 << 1, 0},  {83 << 1, 0},   //  65: 0000 0000 0011x
    {84 << 1, 0},  {85 << 1, 0},   //  66: 0000 0000 0100x
    {86 << 1, 0},  {87 << 1, 0},   //  67: 0000 0000 0101x
    {88 << 1, 0},  {89 << 1, 0},   //  68: 0000 0000 0110x
    {90 << 1, 0},  {91 << 1, 0},   //  69: 0000 0000 0111x
    {0, 0x0a02},   {0, 0x0902},    //  70: 0000 0000 1000x
    {0, 0x0503},   {0, 0x0304},    //  71: 0000 0000 1001x
    {0, 0x0205},   {0, 0x0107},    //  72: 0000 0000 1010x
    {0, 0x0106},   {0, 0x000f},    //  73: 0000 0000 1011x
    {0, 0x000e},   {0, 0x000d},    //  74: 0000 0000 1100x
    {0, 0x000c},   {0, 0x1a01},    //  75: 0000 0000 1101x
    {0, 0x1901},   {0, 0x1801},    //  76: 0000 0000 1110x
    {0, 0x1701},   {0, 0x1601},    //  77: 0000 0000 1111x
    {92 << 1, 0},  {93 << 1, 0},   //  78: 0000 0000 0001 0x
    {94 << 1, 0},  {95 << 1, 0},   //  79: 0000 0000 0001 1x
    {96 << 1, 0},  {97 << 1, 0},   //  80: 0000 0000 0010 0x
    {98 << 1, 0},  {99 << 1, 0},   //  81: 0000 0000 0010 1x
    {100 << 1, 0}, {101 << 1, 0},  //  82: 0000 0000 0011 0x
    {102 << 1, 0}, {103 << 1, 0},  //  83: 0000 0000 0011 1x
    {0, 0x001f},   {0, 0x001e},    //  84: 0000 0000 0100 0x
    {0, 0x001d},   {0, 0x001c},    //  85: 0000 0000 0100 1x
    {0, 0x001b},   {0, 0x001a},    //  86: 0000 0000 0101 0x
    {0, 0x0019},   {0, 0x0018},    //  87: 0000 0000 0101 1x
    {0, 0x0017},   {0, 0x0016},    //  88: 0000 0000 0110 0x
    {0, 0x0015},   {0, 0x0014},    //  89: 0000 0000 0110 1x
    {0, 0x0013},   {0, 0x0012},    //  90: 0000 0000 0111 0x
    {0, 0x0011},   {0, 0x0010},    //  91: 0000 0000 0111 1x
    {104 << 1, 0}, {105 << 1, 0},  //  92: 0000 0000 0001 00x
    {106 << 1, 0}, {107 << 1, 0},  //  93: 0000 0000 0001 01x
    {108 << 1, 0}, {109 << 1, 0},  //  94: 0000 0000 0001 10x
    {110 << 1, 0}, {111 << 1, 0},  //  95: 0000 0000 0001 11x
    {0, 0x0028},   {0, 0x0027},    //  96: 0000 0000 0010 00x
    {0, 0x0026},   {0, 0x0025},    //  97: 0000 0000 0010 01x
    {0, 0x0024},   {0, 0x0023},    //  98: 0000 0000 0010 10x
    {0, 0x0022},   {0, 0x0021},    //  99: 0000 0000 0010 11x
    {0, 0x0020},   {0, 0x010e},    // 100: 0000 0000 0011 00x
    {0, 0x010d},   {0, 0x010c},    // 101: 0000 0000 0011 01x
    {0, 0x010b},   {0, 0x010a},    // 102: 0000 0000 0011 10x
    {0, 0x0109},   {0, 0x0108},    // 103: 0000 0000 0011 11x
    {0, 0x0112},   {0, 0x0111},    // 104: 0000 0000 0001 000x
    {0, 0x0110},   {0, 0x010f},    // 105: 0000 0000 0001 001x
    {0, 0x0603},   {0, 0x1002},    // 106: 0000 0000 0001 010x
    {0, 0x0f02},   {0, 0x0e02},    // 107: 0000 0000 0001 011x
    {0, 0x0d02},   {0, 0x0c02},    // 108: 0000 0000 0001 100x
    {0, 0x0b02},   {0, 0x1f01},    // 109: 0000 0000 0001 101x
    {0, 0x1e01},   {0, 0x1d01},    // 110: 0000 0000 0001 110x
    {0, 0x1c01},   {0, 0x1b01},    // 111: 0000 0000 0001 111x
};

long plmpegdecode_latency_;

static plm_vlc_t *PLM_VIDEO_MACROBLOCK_TYPE[4];
static plm_vlc_t *PLM_VIDEO_DCT_SIZE[3];

#define plm_clamp(n) MIN(255, MAX(0, n))

void plm_video_destroy(plm_video_t *self) {
  if (self->destroy_buffer_when_done) {
    plm_buffer_destroy(self->buffer);
  }
  if (self->has_sequence_header) {
    free(self->frames_data);
  }
  free(self);
}

double plm_video_get_pixel_aspect_ratio(plm_video_t *self) {
  return self->pixel_aspect_ratio;
}

double plm_video_get_framerate(plm_video_t *self) {
  return self->framerate;
}

int plm_video_get_width(plm_video_t *self) {
  return self->width;
}

int plm_video_get_height(plm_video_t *self) {
  return self->height;
}

void plm_video_set_no_delay(plm_video_t *self, int no_delay) {
  self->assume_no_b_frames = no_delay;
}

double plm_video_get_time(plm_video_t *self) {
  return self->time;
}

void plm_video_rewind(plm_video_t *self) {
  plm_buffer_rewind(self->buffer);
  self->time = 0;
  self->frames_decoded = 0;
  self->has_reference_frame = false;
}

void plm_video_init_frame(plm_video_t *self, plm_frame_t *frame,
                          uint8_t *base) {
  size_t plane_size = self->luma_width * self->luma_height;
  frame->width = self->width;
  frame->height = self->height;
  frame->y.width = self->luma_width;
  frame->y.height = self->luma_height;
  frame->y.data = base;
  frame->cr.width = self->chroma_width;
  frame->cr.height = self->chroma_height;
  frame->cr.data = base + plane_size;
  frame->cb.width = self->chroma_width;
  frame->cb.height = self->chroma_height;
  frame->cb.data = base + plane_size * 2;
}

void plm_video_decode_sequence_header(plm_video_t *self) {
  int previous_width = self->width;
  int previous_height = self->height;

  self->width = plm_buffer_read(self->buffer, 12);
  self->height = plm_buffer_read(self->buffer, 12);

  int pixel_aspect_ratio_code;
  pixel_aspect_ratio_code = plm_buffer_read(self->buffer, 4);
  pixel_aspect_ratio_code -= 1;
  pixel_aspect_ratio_code = MAX(pixel_aspect_ratio_code, 0);
  pixel_aspect_ratio_code =
      MIN(pixel_aspect_ratio_code, ARRAYLEN(PLM_VIDEO_PIXEL_ASPECT_RATIO) - 1);
  self->pixel_aspect_ratio =
      PLM_VIDEO_PIXEL_ASPECT_RATIO[pixel_aspect_ratio_code];

  int framerate_code;
  framerate_code = plm_buffer_read(self->buffer, 4);
  framerate_code -= 1;
  framerate_code = MAX(framerate_code, 0);
  framerate_code = MIN(framerate_code, ARRAYLEN(PLM_VIDEO_PICTURE_RATE) - 1);
  self->framerate = PLM_VIDEO_PICTURE_RATE[framerate_code];

  // skip bitRate, marker, bufferSize and constrained bit
  plm_buffer_skip(self->buffer, 18 + 1 + 10 + 1);

  if (plm_buffer_read(self->buffer, 1)) {  // load custom intra quant matrix?
    for (int i = 0; i < 64; i++) {
      int idx = PLM_VIDEO_ZIG_ZAG[i];
      self->intra_quant_matrix[idx] = plm_buffer_read(self->buffer, 8);
    }
  } else {
    memcpy(self->intra_quant_matrix, PLM_VIDEO_INTRAQUANT_MATRIX, 64);
  }

  if (plm_buffer_read(self->buffer,
                      1)) {  // load custom non intra quant matrix?
    for (int i = 0; i < 64; i++) {
      int idx = PLM_VIDEO_ZIG_ZAG[i];
      self->non_intra_quant_matrix[idx] = plm_buffer_read(self->buffer, 8);
    }
  } else {
    memcpy(self->non_intra_quant_matrix, PLM_VIDEO_NONINTRAQUANT_MATRIX, 64);
  }

  if (self->has_sequence_header) {
    if (self->width == previous_width && self->height == previous_height) {
      // We already had a sequence header with the same width/height;
      // nothing else to do here.
      return;
    }

    // We had a sequence header but with different dimensions;
    // delete the previous planes and allocate new.
    free(self->frames_data);
  }

  self->mb_width = (self->width + 15) >> 4;
  self->mb_height = (self->height + 15) >> 4;
  self->mb_size = self->mb_width * self->mb_height;
  self->luma_width = self->mb_width << 4;
  self->luma_height = self->mb_height << 4;
  self->chroma_width = self->mb_width << 3;
  self->chroma_height = self->mb_height << 3;

  size_t plane_size = self->luma_width * self->luma_height;
  self->frames_data = memalign(64, plane_size * 9);
  plm_video_init_frame(self, &self->frame_current,
                       self->frames_data + plane_size * 0);
  plm_video_init_frame(self, &self->frame_forward,
                       self->frames_data + plane_size * 3);
  plm_video_init_frame(self, &self->frame_backward,
                       self->frames_data + plane_size * 6);

  self->has_sequence_header = true;

  INFOF("%s:\n"
        "\t%-20s = %15d;\n"
        "\t%-20s = %15d;\n"
        "\t%-20s = %15f;\n"
        "\t%-20s = %15f;\n"
        "\t%-20s = %15d;\n"
        "\t%-20s = %15d;\n"
        "\t%-20s = %15d;\n"
        "\t%-20s = %15d;\n"
        "\t%-20s = %15d;\n"
        "\t%-20s = %15d;\n"
        "\t%-20s = %15d;",
        "New MPEG Sequence", "width", self->width, "height", self->height,
        "framerate", self->framerate, "pixel_aspect_ratio",
        self->pixel_aspect_ratio, "mb_size", self->mb_size, "mb_width",
        self->mb_width, "mb_height", self->mb_height, "luma_width",
        self->luma_width, "luma_height", self->luma_height, "chroma_width",
        self->chroma_width, "chroma_height", self->chroma_height);
}

static void plm_video_copy_macroblock(plm_video_t *self, int motion_h,
                                      int motion_v, plm_frame_t *d) {
  plm_frame_t *s = &self->frame_current;
  plm_video_process_macroblock_16(self, s->y.data, d->y.data, motion_h,
                                  motion_v, false);
  plm_video_process_macroblock_8(self, s->cr.data, d->cr.data, motion_h / 2,
                                 motion_v / 2, false);
  plm_video_process_macroblock_8(self, s->cb.data, d->cb.data, motion_h / 2,
                                 motion_v / 2, false);
}

static void plm_video_interpolate_macroblock(plm_video_t *self, int motion_h,
                                             int motion_v, plm_frame_t *d) {
  plm_frame_t *s = &self->frame_current;
  plm_video_process_macroblock_16(self, s->y.data, d->y.data, motion_h,
                                  motion_v, true);
  plm_video_process_macroblock_8(self, s->cr.data, d->cr.data, motion_h / 2,
                                 motion_v / 2, true);
  plm_video_process_macroblock_8(self, s->cb.data, d->cb.data, motion_h / 2,
                                 motion_v / 2, true);
}

static int plm_video_decode_motion_vector(plm_video_t *self, int r_size,
                                          int motion) {
  int fscale = 1u << r_size;
  int m_code = plm_buffer_read_vlc(self->buffer, PLM_VIDEO_MOTION);
  int r = 0;
  int d;
  if ((m_code != 0) && (fscale != 1)) {
    r = plm_buffer_read(self->buffer, r_size);
    d = ((abs(m_code) - 1) << r_size) + r + 1;
    if (m_code < 0) {
      d = -d;
    }
  } else {
    d = m_code;
  }
  motion += d;
  if (motion > (fscale << 4) - 1) {
    motion -= fscale << 5;
  } else if (motion < (int)(((unsigned)-fscale) << 4)) {
    motion += fscale << 5;
  }
  return motion;
}

static void plm_video_decode_motion_vectors(plm_video_t *self) {
  // Forward
  if (self->motion_forward.is_set) {
    int r_size = self->motion_forward.r_size;
    self->motion_forward.h =
        plm_video_decode_motion_vector(self, r_size, self->motion_forward.h);
    self->motion_forward.v =
        plm_video_decode_motion_vector(self, r_size, self->motion_forward.v);
  } else if (self->picture_type == PLM_VIDEO_PICTURE_TYPE_PREDICTIVE) {
    // No motion information in P-picture, reset vectors
    self->motion_forward.h = 0;
    self->motion_forward.v = 0;
  }
  if (self->motion_backward.is_set) {
    int r_size = self->motion_backward.r_size;
    self->motion_backward.h =
        plm_video_decode_motion_vector(self, r_size, self->motion_backward.h);
    self->motion_backward.v =
        plm_video_decode_motion_vector(self, r_size, self->motion_backward.v);
  }
}

static void plm_video_predict_macroblock(plm_video_t *self) {
  int fw_h = self->motion_forward.h;
  int fw_v = self->motion_forward.v;
  if (self->motion_forward.full_px) {
    fw_h <<= 1;
    fw_v <<= 1;
  }
  if (self->picture_type == PLM_VIDEO_PICTURE_TYPE_B) {
    int bw_h = self->motion_backward.h;
    int bw_v = self->motion_backward.v;
    if (self->motion_backward.full_px) {
      bw_h <<= 1;
      bw_v <<= 1;
    }
    if (self->motion_forward.is_set) {
      plm_video_copy_macroblock(self, fw_h, fw_v, &self->frame_forward);
      if (self->motion_backward.is_set) {
        plm_video_interpolate_macroblock(self, bw_h, bw_v,
                                         &self->frame_backward);
      }
    } else {
      plm_video_copy_macroblock(self, bw_h, bw_v, &self->frame_backward);
    }
  } else {
    plm_video_copy_macroblock(self, fw_h, fw_v, &self->frame_forward);
  }
}

static void plm_video_decode_block(plm_video_t *self, int block) {
  int n = 0;
  uint8_t *quant_matrix;

  // Decode DC coefficient of intra-coded blocks
  if (self->macroblock_intra) {
    int predictor;
    int dct_size;

    // DC prediction
    int plane_index = block > 3 ? block - 3 : 0;
    predictor = self->dc_predictor[plane_index];
    dct_size =
        plm_buffer_read_vlc(self->buffer, PLM_VIDEO_DCT_SIZE[plane_index]);

    // Read DC coeff
    if (dct_size > 0) {
      int differential = plm_buffer_read(self->buffer, dct_size);
      if ((differential & (1 << (dct_size - 1))) != 0) {
        self->block_data[0] = predictor + differential;
      } else {
        self->block_data[0] =
            predictor + ((-1u << dct_size) | (differential + 1));
      }
    } else {
      self->block_data[0] = predictor;
    }

    // Save predictor value
    self->dc_predictor[plane_index] = self->block_data[0];

    // Dequantize + premultiply
    self->block_data[0] <<= (3 + 5);

    quant_matrix = self->intra_quant_matrix;
    n = 1;
  } else {
    quant_matrix = self->non_intra_quant_matrix;
  }

  // Decode AC coefficients (+DC for non-intra)
  int level = 0;
  while (true) {
    int run = 0;
    uint16_t coeff =
        plm_buffer_read_vlc_uint(self->buffer, PLM_VIDEO_DCT_COEFF);

    if ((coeff == 0x0001) && (n > 0) &&
        (plm_buffer_read(self->buffer, 1) == 0)) {
      // end_of_block
      break;
    }
    if (coeff == 0xffff) {
      // escape
      run = plm_buffer_read(self->buffer, 6);
      level = plm_buffer_read(self->buffer, 8);
      if (level == 0) {
        level = plm_buffer_read(self->buffer, 8);
      } else if (level == 128) {
        level = plm_buffer_read(self->buffer, 8) - 256;
      } else if (level > 128) {
        level = level - 256;
      }
    } else {
      run = coeff >> 8;
      level = coeff & 0xff;
      if (plm_buffer_read(self->buffer, 1)) {
        level = -level;
      }
    }

    n += run;
    if (n < 0 || n >= 64) {
      return;  // invalid
    }

    int de_zig_zagged = PLM_VIDEO_ZIG_ZAG[n];
    n++;

    // Dequantize, oddify, clip
    level = (unsigned)level << 1;
    if (!self->macroblock_intra) {
      level += (level < 0 ? -1 : 1);
    }
    level = (level * self->quantizer_scale * quant_matrix[de_zig_zagged]) >> 4;
    if ((level & 1) == 0) {
      level -= level > 0 ? 1 : -1;
    }
    if (level > 2047) {
      level = 2047;
    } else if (level < -2048) {
      level = -2048;
    }

    // Save premultiplied coefficient
    self->block_data[de_zig_zagged] =
        level * PLM_VIDEO_PREMULTIPLIER_MATRIX[de_zig_zagged];
  }

  // Move block to its place
  uint8_t *d;
  int dw;
  int di;

  if (block < 4) {
    d = self->frame_current.y.data;
    dw = self->luma_width;
    di = (self->mb_row * self->luma_width + self->mb_col) << 4;
    if ((block & 1) != 0) {
      di += 8;
    }
    if ((block & 2) != 0) {
      di += self->luma_width << 3;
    }
  } else {
    d = (block == 4) ? self->frame_current.cb.data
                     : self->frame_current.cr.data;
    dw = self->chroma_width;
    di = ((self->mb_row * self->luma_width) << 2) + (self->mb_col << 3);
  }

  int *s = self->block_data;
  int si = 0;
  if (self->macroblock_intra) {
    // Overwrite (no prediction)
    if (n == 1) {
      int clamped = plm_clamp((s[0] + 128) >> 8);
      PLM_BLOCK_SET(d, di, dw, si, 8, 8, clamped);
      s[0] = 0;
    } else {
      plm_video_idct(s);
      PLM_BLOCK_SET(d, di, dw, si, 8, 8, plm_clamp(s[si]));
      memset(self->block_data, 0, sizeof(self->block_data));
    }
  } else {
    // Add data to the predicted macroblock
    if (n == 1) {
      int value = (s[0] + 128) >> 8;
      PLM_BLOCK_SET(d, di, dw, si, 8, 8, plm_clamp(d[di] + value));
      s[0] = 0;
    } else {
      plm_video_idct(s);
      PLM_BLOCK_SET(d, di, dw, si, 8, 8, plm_clamp(d[di] + s[si]));
      memset(self->block_data, 0, sizeof(self->block_data));
    }
  }
}

static void plm_video_decode_macroblock(plm_video_t *self) {
  // Decode self->macroblock_address_increment
  int increment = 0;
  int t =
      plm_buffer_read_vlc(self->buffer, PLM_VIDEO_MACROBLOCK_ADDRESS_INCREMENT);

  while (t == 34) {
    // macroblock_stuffing
    t = plm_buffer_read_vlc(self->buffer,
                            PLM_VIDEO_MACROBLOCK_ADDRESS_INCREMENT);
  }
  while (t == 35) {
    // macroblock_escape
    increment += 33;
    t = plm_buffer_read_vlc(self->buffer,
                            PLM_VIDEO_MACROBLOCK_ADDRESS_INCREMENT);
  }
  increment += t;

  // Process any skipped macroblocks
  if (self->slice_begin) {
    // The first self->macroblock_address_increment of each slice is relative
    // to beginning of the preverious row, not the preverious macroblock
    self->slice_begin = false;
    self->macroblock_address += increment;
  } else {
    if (self->macroblock_address + increment >= self->mb_size) {
      return;  // invalid
    }
    if (increment > 1) {
      // Skipped macroblocks reset DC predictors
      self->dc_predictor[0] = 128;
      self->dc_predictor[1] = 128;
      self->dc_predictor[2] = 128;

      // Skipped macroblocks in P-pictures reset motion vectors
      if (self->picture_type == PLM_VIDEO_PICTURE_TYPE_PREDICTIVE) {
        self->motion_forward.h = 0;
        self->motion_forward.v = 0;
      }
    }

    // Predict skipped macroblocks
    while (increment > 1) {
      self->macroblock_address++;
      self->mb_row = self->macroblock_address / self->mb_width;
      self->mb_col = self->macroblock_address % self->mb_width;

      plm_video_predict_macroblock(self);
      increment--;
    }
    self->macroblock_address++;
  }

  self->mb_row = self->macroblock_address / self->mb_width;
  self->mb_col = self->macroblock_address % self->mb_width;

  if (self->mb_col >= self->mb_width || self->mb_row >= self->mb_height) {
    return;  // corrupt stream;
  }

  // Process the current macroblock
  // static const s16 *mbTable = MACROBLOCK_TYPE[self->picture_type];
  // macroblock_type = read_huffman(self->bits, mbTable);

  const plm_vlc_t *table = PLM_VIDEO_MACROBLOCK_TYPE[self->picture_type];
  self->macroblock_type = plm_buffer_read_vlc(self->buffer, table);

  self->macroblock_intra = (self->macroblock_type & 0x01);
  self->motion_forward.is_set = (self->macroblock_type & 0x08);
  self->motion_backward.is_set = (self->macroblock_type & 0x04);

  // Quantizer scale
  if ((self->macroblock_type & 0x10) != 0) {
    self->quantizer_scale = plm_buffer_read(self->buffer, 5);
  }

  if (self->macroblock_intra) {
    // Intra-coded macroblocks reset motion vectors
    self->motion_backward.h = self->motion_forward.h = 0;
    self->motion_backward.v = self->motion_forward.v = 0;
  } else {
    // Non-intra macroblocks reset DC predictors
    self->dc_predictor[0] = 128;
    self->dc_predictor[1] = 128;
    self->dc_predictor[2] = 128;

    plm_video_decode_motion_vectors(self);
    plm_video_predict_macroblock(self);
  }

  // Decode blocks
  int cbp =
      ((self->macroblock_type & 0x02) != 0)
          ? plm_buffer_read_vlc(self->buffer, PLM_VIDEO_CODE_BLOCK_PATTERN)
          : (self->macroblock_intra ? 0x3f : 0);

  for (int block = 0, mask = 0x20; block < 6; block++) {
    if ((cbp & mask) != 0) {
      plm_video_decode_block(self, block);
    }
    mask >>= 1;
  }
}

static void plm_video_decode_slice(plm_video_t *self, int slice) {
  self->slice_begin = true;
  self->macroblock_address = (slice - 1) * self->mb_width - 1;
  // Reset motion vectors and DC predictors
  self->motion_backward.h = self->motion_forward.h = 0;
  self->motion_backward.v = self->motion_forward.v = 0;
  self->dc_predictor[0] = 128;
  self->dc_predictor[1] = 128;
  self->dc_predictor[2] = 128;
  self->quantizer_scale = plm_buffer_read(self->buffer, 5);
  // Skip extra
  while (plm_buffer_read(self->buffer, 1)) {
    plm_buffer_skip(self->buffer, 8);
  }
  do {
    plm_video_decode_macroblock(self);
  } while (self->macroblock_address < self->mb_size - 1 &&
           plm_buffer_no_start_code(self->buffer));
}

static void plm_video_decode_picture(plm_video_t *self) {
  plm_buffer_skip(self->buffer, 10);  // skip temporalReference
  self->picture_type = plm_buffer_read(self->buffer, 3);
  plm_buffer_skip(self->buffer, 16);  // skip vbv_delay

  // D frames or unknown coding type
  if (self->picture_type <= 0 ||
      self->picture_type > PLM_VIDEO_PICTURE_TYPE_B) {
    return;
  }

  // forward full_px, f_code
  if (self->picture_type == PLM_VIDEO_PICTURE_TYPE_PREDICTIVE ||
      self->picture_type == PLM_VIDEO_PICTURE_TYPE_B) {
    self->motion_forward.full_px = plm_buffer_read(self->buffer, 1);
    int f_code = plm_buffer_read(self->buffer, 3);
    if (f_code == 0) {
      // Ignore picture with zero f_code
      return;
    }
    self->motion_forward.r_size = f_code - 1;
  }

  // backward full_px, f_code
  if (self->picture_type == PLM_VIDEO_PICTURE_TYPE_B) {
    self->motion_backward.full_px = plm_buffer_read(self->buffer, 1);
    int f_code = plm_buffer_read(self->buffer, 3);
    if (f_code == 0) {
      // Ignore picture with zero f_code
      return;
    }
    self->motion_backward.r_size = f_code - 1;
  }

  plm_frame_t frame_temp = self->frame_forward;

  if (self->picture_type == PLM_VIDEO_PICTURE_TYPE_INTRA ||
      self->picture_type == PLM_VIDEO_PICTURE_TYPE_PREDICTIVE) {
    self->frame_forward = self->frame_backward;
  }

  // Skip extensions, user data
  do {
    self->start_code = plm_buffer_next_start_code(self->buffer);
  } while (self->start_code == PLM_START_EXTENSION ||
           self->start_code == PLM_START_USER_DATA);

  while (self->start_code >= PLM_START_SLICE_FIRST &&
         self->start_code <= PLM_START_SLICE_LAST) {
    plm_video_decode_slice(self, self->start_code & 0x000000FF);
    if (self->macroblock_address == self->mb_size - 1) {
      break;
    }
    self->start_code = plm_buffer_next_start_code(self->buffer);
  }

  // If this is a reference picutre rotate the prediction pointers
  if (self->picture_type == PLM_VIDEO_PICTURE_TYPE_INTRA ||
      self->picture_type == PLM_VIDEO_PICTURE_TYPE_PREDICTIVE) {
    self->frame_backward = self->frame_current;
    self->frame_current = frame_temp;
  }
}

static plm_frame_t *plm_video_decode_impl(plm_video_t *self) {
  plm_frame_t *frame = NULL;
  if (!self->has_sequence_header) {
    self->start_code =
        plm_buffer_find_start_code(self->buffer, PLM_START_SEQUENCE);
    if (self->start_code == -1) {
      return NULL;
    }
    plm_video_decode_sequence_header(self);
  }
  do {
    if (self->start_code != PLM_START_PICTURE) {
      self->start_code =
          plm_buffer_find_start_code(self->buffer, PLM_START_PICTURE);
    }
    if (self->start_code == -1) {
      return NULL;
    }
    plm_video_decode_picture(self);
    if (self->assume_no_b_frames) {
      frame = &self->frame_backward;
    } else if (self->picture_type == PLM_VIDEO_PICTURE_TYPE_B) {
      frame = &self->frame_current;
    } else if (self->has_reference_frame) {
      frame = &self->frame_forward;
    } else {
      self->has_reference_frame = true;
    }
  } while (!frame);
  frame->time = self->time;
  self->frames_decoded++;
  self->time = (double)self->frames_decoded / self->framerate;
  return frame;
}

plm_frame_t *plm_video_decode(plm_video_t *self) {
  plm_frame_t *res;
  struct timespec tsc;
  INFOF("plm_video_decode");
  tsc = timespec_real();
  res = plm_video_decode_impl(self);
  plmpegdecode_latency_ = timespec_tomicros(timespec_sub(timespec_real(), tsc));
  return res;
}

plm_video_t *plm_video_create_with_buffer(plm_buffer_t *buffer,
                                          int destroy_when_done) {
  plm_video_t *self = (plm_video_t *)memalign(64, sizeof(plm_video_t));
  memset(self, 0, sizeof(plm_video_t));
  self->buffer = buffer;
  self->destroy_buffer_when_done = destroy_when_done;
  self->start_code =
      plm_buffer_find_start_code(self->buffer, PLM_START_SEQUENCE);
  if (self->start_code != -1) {
    plm_video_decode_sequence_header(self);
  }
  return self;
}

static textstartup void plm_video_init(void) {
  PLM_VIDEO_MACROBLOCK_TYPE[0] = NULL;
  PLM_VIDEO_MACROBLOCK_TYPE[1] = (void *)PLM_VIDEO_MACROBLOCK_TYPE_INTRA;
  PLM_VIDEO_MACROBLOCK_TYPE[2] = (void *)PLM_VIDEO_MACROBLOCK_TYPE_PREDICTIVE;
  PLM_VIDEO_MACROBLOCK_TYPE[3] = (void *)PLM_VIDEO_MACROBLOCK_TYPE_B;
  PLM_VIDEO_DCT_SIZE[0] = (void *)PLM_VIDEO_DCT_SIZE_LUMINANCE;
  PLM_VIDEO_DCT_SIZE[1] = (void *)PLM_VIDEO_DCT_SIZE_CHROMINANCE;
  PLM_VIDEO_DCT_SIZE[2] = (void *)PLM_VIDEO_DCT_SIZE_CHROMINANCE;
}

const void *const plm_video_init_ctor[] initarray = {plm_video_init};
