#ifndef COSMOPOLITAN_THIRD_PARTY_STB_STB_IMAGE_H_
#define COSMOPOLITAN_THIRD_PARTY_STB_STB_IMAGE_H_
#include "libc/stdio/stdio.h"
COSMOPOLITAN_C_START_

enum {
  STBI_default = 0,  // only used for desired_channels
  STBI_grey = 1,
  STBI_grey_alpha = 2,
  STBI_rgb = 3,
  STBI_rgb_alpha = 4
};

typedef struct {
  // fill 'data' with 'size' bytes.  return number of bytes actually read
  int (*read)(void *user, char *data, int size);

  // skip the next 'n' bytes, or 'unget' the last -n bytes if negative
  void (*skip)(void *user, int n);

  // returns nonzero if we are at end of file/data
  int (*eof)(void *user);
} stbi_io_callbacks;

//
// 8-bits-per-channel interface
//

unsigned char *stbi_load_from_memory(unsigned char const *buffer, int len,
                                     int *x, int *y, int *channels_in_file,
                                     int desired_channels) mallocesque;
unsigned char *stbi_load_from_callbacks(stbi_io_callbacks const *clbk,
                                        void *user, int *x, int *y,
                                        int *channels_in_file,
                                        int desired_channels);

unsigned char *stbi_load(char const *filename, int *x, int *y,
                         int *channels_in_file, int desired_channels);
unsigned char *stbi_load_from_file(FILE *f, int *x, int *y,
                                   int *channels_in_file, int desired_channels);
// for stbi_load_from_file, file pointer is left pointing immediately after
// image

unsigned char *stbi_load_gif_from_memory(unsigned char const *buffer, int len,
                                         int **delays, int *x, int *y, int *z,
                                         int *comp, int req_comp);

//
// 16-bits-per-channel interface
//

unsigned short *stbi_load_16_from_memory(unsigned char const *buffer, int len,
                                         int *x, int *y, int *channels_in_file,
                                         int desired_channels);
unsigned short *stbi_load_16_from_callbacks(stbi_io_callbacks const *clbk,
                                            void *user, int *x, int *y,
                                            int *channels_in_file,
                                            int desired_channels);

unsigned short *stbi_load_16(char const *filename, int *x, int *y,
                             int *channels_in_file, int desired_channels);
unsigned short *stbi_load_from_file_16(FILE *f, int *x, int *y,
                                       int *channels_in_file,
                                       int desired_channels);

// get a VERY brief reason for failure
const char *stbi_failure_reason(void);

// free the loaded image -- this is just free()
void stbi_image_free(void *retval_from_stbi_load);

// get image dimensions & components without fully decoding
int stbi_info_from_memory(unsigned char const *buffer, int len, int *x, int *y,
                          int *comp);
int stbi_info_from_callbacks(stbi_io_callbacks const *clbk, void *user, int *x,
                             int *y, int *comp);
int stbi_is_16_bit_from_memory(unsigned char const *buffer, int len);
int stbi_is_16_bit_from_callbacks(stbi_io_callbacks const *clbk, void *user);

int stbi_info(char const *filename, int *x, int *y, int *comp);
int stbi_info_from_file(FILE *f, int *x, int *y, int *comp);
int stbi_is_16_bit(char const *filename);
int stbi_is_16_bit_from_file(FILE *f);

// for image formats that explicitly notate that they have premultiplied alpha,
// we just return the colors as stored in the file. set this flag to force
// unpremultiplication. results are undefined if the unpremultiply overflow.
void stbi_set_unpremultiply_on_load(int flag_true_if_should_unpremultiply);

// indicate whether we should process iphone images back to canonical format,
// or just pass them through "as-is"
void stbi_convert_iphone_png_to_rgb(int flag_true_if_should_convert);

// flip the image vertically, so the first pixel in the output array is the
// bottom left
void stbi_set_flip_vertically_on_load(int flag_true_if_should_flip);

// ZLIB client - used by PNG, available for other purposes

char *stbi_zlib_decode_malloc_guesssize(const char *buffer, int len,
                                        int initial_size, int *outlen);
char *stbi_zlib_decode_malloc_guesssize_headerflag(const char *buffer, int len,
                                                   int initial_size,
                                                   int *outlen,
                                                   int parse_header);
char *stbi_zlib_decode_malloc(const char *buffer, int len, int *outlen);
int stbi_zlib_decode_buffer(char *obuffer, int olen, const char *ibuffer,
                            int ilen);

char *stbi_zlib_decode_noheader_malloc(const char *buffer, int len,
                                       int *outlen);
int stbi_zlib_decode_noheader_buffer(char *obuffer, int olen,
                                     const char *ibuffer, int ilen);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_STB_STB_IMAGE_H_ */
