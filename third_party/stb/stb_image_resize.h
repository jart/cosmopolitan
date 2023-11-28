#ifndef COSMOPOLITAN_THIRD_PARTY_STB_STB_IMAGE_RESIZE_H_
#define COSMOPOLITAN_THIRD_PARTY_STB_STB_IMAGE_RESIZE_H_
COSMOPOLITAN_C_START_

// Easy-to-use API:
//
//     * "input pixels" points to an array of image data with 'num_channels'
//     channels (e.g. RGB=3, RGBA=4)
//     * input_w is input image width (x-axis), input_h is input image height
//     (y-axis)
//     * stride is the offset between successive rows of image data in memory,
//     in bytes. you can
//       specify 0 to mean packed continuously in memory
//     * alpha channel is treated identically to other channels.
//     * colorspace is linear or sRGB as specified by function name
//     * returned result is 1 for success or 0 in case of an error.
//       #define STBIR_ASSERT() to trigger an assert on parameter validation
//       errors.
//     * Memory required grows approximately linearly with input and output
//     size, but with
//       discontinuities at input_w == output_w and input_h == output_h.
//     * These functions use a "default" resampling filter defined at compile
//     time. To change the filter,
//       you can change the compile-time defaults by #defining
//       STBIR_DEFAULT_FILTER_UPSAMPLE and STBIR_DEFAULT_FILTER_DOWNSAMPLE, or
//       you can use the medium-complexity API.

int stbir_resize_uint8(const unsigned char* input_pixels, int input_w,
                       int input_h, int input_stride_in_bytes,
                       unsigned char* output_pixels, int output_w, int output_h,
                       int output_stride_in_bytes, int num_channels);

int stbir_resize_float(const float* input_pixels, int input_w, int input_h,
                       int input_stride_in_bytes, float* output_pixels,
                       int output_w, int output_h, int output_stride_in_bytes,
                       int num_channels);

// The following functions interpret image data as gamma-corrected sRGB.
// Specify STBIR_ALPHA_CHANNEL_NONE if you have no alpha channel,
// or otherwise provide the index of the alpha channel. Flags value
// of 0 will probably do the right thing if you're not sure what
// the flags mean.

#define STBIR_ALPHA_CHANNEL_NONE -1

// Set this flag if your texture has premultiplied alpha. Otherwise, stbir will
// use alpha-weighted resampling (effectively premultiplying, resampling,
// then unpremultiplying).
#define STBIR_FLAG_ALPHA_PREMULTIPLIED (1 << 0)
// The specified alpha channel should be handled as gamma-corrected value even
// when doing sRGB operations.
#define STBIR_FLAG_ALPHA_USES_COLORSPACE (1 << 1)

int stbir_resize_uint8_srgb(const unsigned char* input_pixels, int input_w,
                            int input_h, int input_stride_in_bytes,
                            unsigned char* output_pixels, int output_w,
                            int output_h, int output_stride_in_bytes,
                            int num_channels, int alpha_channel, int flags);

typedef enum {
  STBIR_EDGE_CLAMP = 1,
  STBIR_EDGE_REFLECT = 2,
  STBIR_EDGE_WRAP = 3,
  STBIR_EDGE_ZERO = 4,
} stbir_edge;

// This function adds the ability to specify how requests to sample off the edge
// of the image are handled.
int stbir_resize_uint8_srgb_edgemode(const unsigned char* input_pixels,
                                     int input_w, int input_h,
                                     int input_stride_in_bytes,
                                     unsigned char* output_pixels, int output_w,
                                     int output_h, int output_stride_in_bytes,
                                     int num_channels, int alpha_channel,
                                     int flags, stbir_edge edge_wrap_mode);

// Medium-complexity API
//
// This extends the easy-to-use API as follows:
//
//     * Alpha-channel can be processed separately
//       * If alpha_channel is not STBIR_ALPHA_CHANNEL_NONE
//         * Alpha channel will not be gamma corrected (unless
//         flags&STBIR_FLAG_GAMMA_CORRECT)
//         * Filters will be weighted by alpha channel (unless
//         flags&STBIR_FLAG_ALPHA_PREMULTIPLIED)
//     * Filter can be selected explicitly
//     * uint16 image type
//     * sRGB colorspace available for all types
//     * context parameter for passing to STBIR_MALLOC

typedef enum {
  // use same filter type that easy-to-use API chooses
  STBIR_FILTER_DEFAULT = 0,
  // a trapezoid w/1-pixel wide ramps, same result as box for integer
  // scale ratios
  STBIR_FILTER_BOX = 1,
  // On upsampling, produces same results as bilinear texture filtering
  STBIR_FILTER_TRIANGLE = 2,
  // The cubic b-spline (aka Mitchell-Netrevalli with B=1,C=0), gaussian-esque
  STBIR_FILTER_CUBICBSPLINE = 3,
  // An interpolating cubic spline
  STBIR_FILTER_CATMULLROM = 4,
  // Mitchell-Netrevalli filter with B=1/3, C=1/3
  STBIR_FILTER_MITCHELL = 5,
} stbir_filter;

typedef enum {
  STBIR_COLORSPACE_LINEAR,
  STBIR_COLORSPACE_SRGB,
  STBIR_MAX_COLORSPACES,
} stbir_colorspace;

// The following functions are all identical except for the type of the image
// data

int stbir_resize_uint8_generic(const unsigned char* input_pixels, int input_w,
                               int input_h, int input_stride_in_bytes,
                               unsigned char* output_pixels, int output_w,
                               int output_h, int output_stride_in_bytes,
                               int num_channels, int alpha_channel, int flags,
                               stbir_edge edge_wrap_mode, stbir_filter filter,
                               stbir_colorspace space, void* alloc_context);

int stbir_resize_uint16_generic(const uint16_t* input_pixels, int input_w,
                                int input_h, int input_stride_in_bytes,
                                uint16_t* output_pixels, int output_w,
                                int output_h, int output_stride_in_bytes,
                                int num_channels, int alpha_channel, int flags,
                                stbir_edge edge_wrap_mode, stbir_filter filter,
                                stbir_colorspace space, void* alloc_context);

int stbir_resize_float_generic(const float* input_pixels, int input_w,
                               int input_h, int input_stride_in_bytes,
                               float* output_pixels, int output_w, int output_h,
                               int output_stride_in_bytes, int num_channels,
                               int alpha_channel, int flags,
                               stbir_edge edge_wrap_mode, stbir_filter filter,
                               stbir_colorspace space, void* alloc_context);

// Full-complexity API
//
// This extends the medium API as follows:
//
//     * uint32 image type
//     * not typesafe
//     * separate filter types for each axis
//     * separate edge modes for each axis
//     * can specify scale explicitly for subpixel correctness
//     * can specify image source tile using texture coordinates

typedef enum {
  STBIR_TYPE_UINT8,
  STBIR_TYPE_UINT16,
  STBIR_TYPE_UINT32,
  STBIR_TYPE_FLOAT,
  STBIR_MAX_TYPES
} stbir_datatype;

int stbir_resize(const void* input_pixels, int input_w, int input_h,
                 int input_stride_in_bytes, void* output_pixels, int output_w,
                 int output_h, int output_stride_in_bytes,
                 stbir_datatype datatype, int num_channels, int alpha_channel,
                 int flags, stbir_edge edge_mode_horizontal,
                 stbir_edge edge_mode_vertical, stbir_filter filter_horizontal,
                 stbir_filter filter_vertical, stbir_colorspace space,
                 void* alloc_context);

int stbir_resize_subpixel(
    const void* input_pixels, int input_w, int input_h,
    int input_stride_in_bytes, void* output_pixels, int output_w, int output_h,
    int output_stride_in_bytes, stbir_datatype datatype, int num_channels,
    int alpha_channel, int flags, stbir_edge edge_mode_horizontal,
    stbir_edge edge_mode_vertical, stbir_filter filter_horizontal,
    stbir_filter filter_vertical, stbir_colorspace space, void* alloc_context,
    float x_scale, float y_scale, float x_offset, float y_offset);

int stbir_resize_region(
    const void* input_pixels, int input_w, int input_h,
    int input_stride_in_bytes, void* output_pixels, int output_w, int output_h,
    int output_stride_in_bytes, stbir_datatype datatype, int num_channels,
    int alpha_channel, int flags, stbir_edge edge_mode_horizontal,
    stbir_edge edge_mode_vertical, stbir_filter filter_horizontal,
    stbir_filter filter_vertical, stbir_colorspace space, void* alloc_context,
    float s0, float t0, float s1, float t1);
// (s0, t0) & (s1, t1) are the top-left and bottom right corner (uv addressing
// style: [0, 1]x[0, 1]) of a region of the input image to use.

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_STB_STB_IMAGE_RESIZE_H_ */
