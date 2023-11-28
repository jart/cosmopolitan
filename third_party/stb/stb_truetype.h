#ifndef COSMOPOLITAN_THIRD_PARTY_STB_STB_TRUETYPE_H_
#define COSMOPOLITAN_THIRD_PARTY_STB_STB_TRUETYPE_H_
#include "libc/runtime/runtime.h"
#include "third_party/stb/stb_rect_pack.h"

#define STBTT_vmove  1
#define STBTT_vline  2
#define STBTT_vcurve 3
#define STBTT_vcubic 4

#define STBTT_MACSTYLE_DONTCARE   0
#define STBTT_MACSTYLE_BOLD       1
#define STBTT_MACSTYLE_ITALIC     2
#define STBTT_MACSTYLE_UNDERSCORE 4
#define STBTT_MACSTYLE_NONE       8

#define STBTT_PLATFORM_ID_UNICODE   0
#define STBTT_PLATFORM_ID_MAC       1
#define STBTT_PLATFORM_ID_ISO       2
#define STBTT_PLATFORM_ID_MICROSOFT 3

#define STBTT_UNICODE_EID_UNICODE_1_0      0
#define STBTT_UNICODE_EID_UNICODE_1_1      1
#define STBTT_UNICODE_EID_ISO_10646        2
#define STBTT_UNICODE_EID_UNICODE_2_0_BMP  3
#define STBTT_UNICODE_EID_UNICODE_2_0_FULL 4

#define STBTT_MS_EID_SYMBOL       0
#define STBTT_MS_EID_UNICODE_BMP  1
#define STBTT_MS_EID_SHIFTJIS     2
#define STBTT_MS_EID_UNICODE_FULL 10

#define STBTT_MAC_EID_ROMAN        0
#define STBTT_MAC_EID_ARABIC       4
#define STBTT_MAC_EID_JAPANESE     1
#define STBTT_MAC_EID_HEBREW       5
#define STBTT_MAC_EID_CHINESE_TRAD 2
#define STBTT_MAC_EID_GREEK        6
#define STBTT_MAC_EID_KOREAN       3
#define STBTT_MAC_EID_RUSSIAN      7

#define STBTT_MS_LANG_ENGLISH  0x0409
#define STBTT_MS_LANG_ITALIAN  0x0410
#define STBTT_MS_LANG_CHINESE  0x0804
#define STBTT_MS_LANG_JAPANESE 0x0411
#define STBTT_MS_LANG_DUTCH    0x0413
#define STBTT_MS_LANG_KOREAN   0x0412
#define STBTT_MS_LANG_FRENCH   0x040c
#define STBTT_MS_LANG_RUSSIAN  0x0419
#define STBTT_MS_LANG_GERMAN   0x0407
#define STBTT_MS_LANG_SPANISH  0x0409
#define STBTT_MS_LANG_HEBREW   0x040d
#define STBTT_MS_LANG_SWEDISH  0x041D

#define STBTT_MAC_LANG_ENGLISH            0
#define STBTT_MAC_LANG_JAPANESE           11
#define STBTT_MAC_LANG_ARABIC             12
#define STBTT_MAC_LANG_KOREAN             23
#define STBTT_MAC_LANG_DUTCH              4
#define STBTT_MAC_LANG_RUSSIAN            32
#define STBTT_MAC_LANG_FRENCH             1
#define STBTT_MAC_LANG_SPANISH            6
#define STBTT_MAC_LANG_GERMAN             2
#define STBTT_MAC_LANG_SWEDISH            5
#define STBTT_MAC_LANG_HEBREW             10
#define STBTT_MAC_LANG_CHINESE_SIMPLIFIED 33
#define STBTT_MAC_LANG_ITALIAN            3
#define STBTT_MAC_LANG_CHINESE_TRAD       19

#define STBTT_POINT_SIZE(x) (-(x))

COSMOPOLITAN_C_START_

typedef int16_t stbtt_vertex_type;
typedef struct stbtt_fontinfo stbtt_fontinfo;
typedef struct stbtt_pack_context stbtt_pack_context;

typedef struct {
  unsigned char *data;
  int cursor;
  int size;
} stbtt__buf;

typedef struct {
  unsigned short x0, y0, x1, y1;
  float xoff, yoff, xadvance;
} stbtt_bakedchar;

typedef struct {
  float x0, y0, s0, t0;
  float x1, y1, s1, t1;
} stbtt_aligned_quad;

typedef struct {
  unsigned short x0, y0, x1, y1;
  float xoff, yoff, xadvance;
  float xoff2, yoff2;
} stbtt_packedchar;

typedef struct {
  int w, h, stride;
  unsigned char *pixels;
} stbtt__bitmap;

typedef struct {
  float font_size;
  int first_unicode_codepoint_in_range;
  int *array_of_unicode_codepoints;
  int num_chars;
  stbtt_packedchar *chardata_for_range;
  unsigned char h_oversample, v_oversample;
} stbtt_pack_range;

struct stbtt_pack_context {
  void *user_allocator_context;
  void *pack_info;
  int width;
  int height;
  int stride_in_bytes;
  int padding;
  int skip_missing;
  unsigned int h_oversample, v_oversample;
  unsigned char *pixels;
  void *nodes;
};

struct stbtt_fontinfo {
  void *userdata;
  int hmtx, kern, gpos, svg;
  int loca, head, glyf, hhea;
  unsigned char *data;     // pointer to .ttf file
  int fontstart;           // offset of start of font
  int numGlyphs;           // needed for range checking
  int index_map;           // cmap mapping for chosen char encoding
  int indexToLocFormat;    // format mapping glyph index to glyph
  stbtt__buf cff;          // cff font data
  stbtt__buf charstrings;  // charstring index
  stbtt__buf gsubrs;       // global charstring subroutines index
  stbtt__buf subrs;        // private charstring subroutines index
  stbtt__buf fontdicts;    // array of font dicts
  stbtt__buf fdselect;     // map from glyph to fontdict
};

typedef struct stbtt_kerningentry {
  int glyph1;  // use stbtt_FindGlyphIndex
  int glyph2;
  int advance;
} stbtt_kerningentry;

typedef struct {
  stbtt_vertex_type x, y, cx, cy, cx1, cy1;
  unsigned char type, padding;
} stbtt_vertex;

extern jmp_buf stbtt_jmpbuf;

int stbtt_BakeFontBitmap(const unsigned char *, int, float, unsigned char *,
                         int, int, int, int, stbtt_bakedchar *);
void stbtt_GetBakedQuad(const stbtt_bakedchar *, int, int, int, float *,
                        float *, stbtt_aligned_quad *, int);
void stbtt_GetScaledFontVMetrics(const unsigned char *, int, float, float *,
                                 float *, float *);
int stbtt_PackBegin(stbtt_pack_context *, unsigned char *, int, int, int, int,
                    void *);
void stbtt_PackEnd(stbtt_pack_context *);
int stbtt_PackFontRange(stbtt_pack_context *, const unsigned char *, int, float,
                        int, int, stbtt_packedchar *);
int stbtt_PackFontRanges(stbtt_pack_context *, const unsigned char *, int,
                         stbtt_pack_range *, int);
void stbtt_PackSetOversampling(stbtt_pack_context *, unsigned int,
                               unsigned int);
void stbtt_PackSetSkipMissingCodepoints(stbtt_pack_context *, int);
void stbtt_GetPackedQuad(const stbtt_packedchar *, int, int, int, float *,
                         float *, stbtt_aligned_quad *, int);
int stbtt_PackFontRangesGatherRects(stbtt_pack_context *,
                                    const stbtt_fontinfo *, stbtt_pack_range *,
                                    int, stbrp_rect *);
void stbtt_PackFontRangesPackRects(stbtt_pack_context *, stbrp_rect *, int);
int stbtt_PackFontRangesRenderIntoRects(stbtt_pack_context *,
                                        const stbtt_fontinfo *,
                                        stbtt_pack_range *, int, stbrp_rect *);
int stbtt_GetNumberOfFonts(const unsigned char *);
int stbtt_GetFontOffsetForIndex(const unsigned char *, int);
int stbtt_InitFont(stbtt_fontinfo *, const unsigned char *, int);
int stbtt_FindGlyphIndex(const stbtt_fontinfo *, int);
float stbtt_ScaleForPixelHeight(const stbtt_fontinfo *, float);
float stbtt_ScaleForMappingEmToPixels(const stbtt_fontinfo *, float);
void stbtt_GetFontVMetrics(const stbtt_fontinfo *, int *, int *, int *);
int stbtt_GetFontVMetricsOS2(const stbtt_fontinfo *, int *, int *, int *);
void stbtt_GetFontBoundingBox(const stbtt_fontinfo *, int *, int *, int *,
                              int *);
void stbtt_GetCodepointHMetrics(const stbtt_fontinfo *, int, int *, int *);
int stbtt_GetCodepointKernAdvance(const stbtt_fontinfo *, int, int);
int stbtt_GetCodepointBox(const stbtt_fontinfo *, int, int *, int *, int *,
                          int *);
void stbtt_GetGlyphHMetrics(const stbtt_fontinfo *, int, int *, int *);
int stbtt_GetGlyphKernAdvance(const stbtt_fontinfo *, int, int);
int stbtt_GetGlyphBox(const stbtt_fontinfo *, int, int *, int *, int *, int *);
int stbtt_GetKerningTableLength(const stbtt_fontinfo *);
int stbtt_GetKerningTable(const stbtt_fontinfo *, stbtt_kerningentry *, int);
int stbtt_IsGlyphEmpty(const stbtt_fontinfo *, int);
int stbtt_GetCodepointShape(const stbtt_fontinfo *, int, stbtt_vertex **);
int stbtt_GetGlyphShape(const stbtt_fontinfo *, int, stbtt_vertex **);
void stbtt_FreeShape(const stbtt_fontinfo *, stbtt_vertex *);
unsigned char *stbtt_FindSVGDoc(const stbtt_fontinfo *, int);
int stbtt_GetCodepointSVG(const stbtt_fontinfo *, int, const char **);
int stbtt_GetGlyphSVG(const stbtt_fontinfo *, int, const char **);
void stbtt_FreeBitmap(unsigned char *, void *);
void *stbtt_GetCodepointBitmap(const stbtt_fontinfo *, float, float, int, int *,
                               int *, int *, int *);
void *stbtt_GetCodepointBitmapSubpixel(const stbtt_fontinfo *, float, float,
                                       float, float, int, int *, int *, int *,
                                       int *);
void stbtt_MakeCodepointBitmap(const stbtt_fontinfo *, unsigned char *, int,
                               int, int, float, float, int);
void stbtt_MakeCodepointBitmapSubpixel(const stbtt_fontinfo *, unsigned char *,
                                       int, int, int, float, float, float,
                                       float, int);
void stbtt_MakeCodepointBitmapSubpixelPrefilter(const stbtt_fontinfo *,
                                                unsigned char *, int, int, int,
                                                float, float, float, float, int,
                                                int, float *, float *, int);
void stbtt_GetCodepointBitmapBox(const stbtt_fontinfo *, int, float, float,
                                 int *, int *, int *, int *);
void stbtt_GetCodepointBitmapBoxSubpixel(const stbtt_fontinfo *, int, float,
                                         float, float, float, int *, int *,
                                         int *, int *);
unsigned char *stbtt_GetGlyphBitmap(const stbtt_fontinfo *, float, float, int,
                                    int *, int *, int *, int *);
unsigned char *stbtt_GetGlyphBitmapSubpixel(const stbtt_fontinfo *, float,
                                            float, float, float, int, int *,
                                            int *, int *, int *);
void stbtt_MakeGlyphBitmap(const stbtt_fontinfo *, unsigned char *, int, int,
                           int, float, float, int);
void stbtt_MakeGlyphBitmapSubpixel(const stbtt_fontinfo *, unsigned char *, int,
                                   int, int, float, float, float, float, int);
void stbtt_MakeGlyphBitmapSubpixelPrefilter(const stbtt_fontinfo *,
                                            unsigned char *, int, int, int,
                                            float, float, float, float, int,
                                            int, float *, float *, int);
void stbtt_GetGlyphBitmapBox(const stbtt_fontinfo *, int, float, float, int *,
                             int *, int *, int *);
void stbtt_GetGlyphBitmapBoxSubpixel(const stbtt_fontinfo *, int, float, float,
                                     float, float, int *, int *, int *, int *);
void stbtt_Rasterize(stbtt__bitmap *, float, stbtt_vertex *, int, float, float,
                     float, float, int, int, int, void *);
void stbtt_FreeSDF(unsigned char *, void *);
unsigned char *stbtt_GetGlyphSDF(const stbtt_fontinfo *, float, int, int,
                                 unsigned char, float, int *, int *, int *,
                                 int *);
unsigned char *stbtt_GetCodepointSDF(const stbtt_fontinfo *, float, int, int,
                                     unsigned char, float, int *, int *, int *,
                                     int *);
int stbtt_FindMatchingFont(const unsigned char *, const char *, int);
int stbtt_CompareUTF8toUTF16_bigendian(const char *, int, const char *, int);
const char *stbtt_GetFontNameString(const stbtt_fontinfo *, int *, int, int,
                                    int, int);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_STB_STB_TRUETYPE_H_ */
