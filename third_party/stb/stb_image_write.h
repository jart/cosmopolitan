#ifndef COSMOPOLITAN_THIRD_PARTY_STB_STB_IMAGE_WRITE_H_
#define COSMOPOLITAN_THIRD_PARTY_STB_STB_IMAGE_WRITE_H_
COSMOPOLITAN_C_START_

extern int stbi_write_png_compression_level;
extern int stbi_write_tga_with_rle;
extern int stbi_write_force_png_filter;

int stbi_write_png(const char *, int, int, int, const void *, int);
int stbi_write_bmp(const char *, int, int, int, const void *);
int stbi_write_tga(const char *, int, int, int, const void *);
int stbi_write_hdr(const char *, int, int, int, const float *);
int stbi_write_jpg(const char *, int, int, int, const void *, int);

typedef void stbi_write_func(void *, void *, int);

int stbi_write_png_to_func(stbi_write_func *, void *, int, int, int,
                           const void *, int);
int stbi_write_bmp_to_func(stbi_write_func *, void *, int, int, int,
                           const void *);
int stbi_write_tga_to_func(stbi_write_func *, void *, int, int, int,
                           const void *);
int stbi_write_hdr_to_func(stbi_write_func *, void *, int, int, int,
                           const float *);
int stbi_write_jpg_to_func(stbi_write_func *, void *, int, int, int,
                           const void *, int);
unsigned char *stbi_write_png_to_mem(const unsigned char *, int, int, int, int,
                                     int *);

void stbi_flip_vertically_on_write(int);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_STB_STB_IMAGE_WRITE_H_ */
