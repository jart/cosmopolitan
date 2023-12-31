#ifndef COSMOPOLITAN_DSP_MPEG_BUFFER_H_
#define COSMOPOLITAN_DSP_MPEG_BUFFER_H_
#include "dsp/mpeg/mpeg.h"
COSMOPOLITAN_C_START_

enum plm_buffer_mode {
  PLM_BUFFER_MODE_FILE,
  PLM_BUFFER_MODE_FIXED_MEM,
  PLM_BUFFER_MODE_DYNAMIC_MEM
};

typedef struct plm_buffer_t {
  unsigned bit_index;
  unsigned capacity;
  unsigned length;
  int free_when_done;
  int close_when_done;
  FILE *fh;
  plm_buffer_load_callback load_callback;
  void *load_callback_user_data;
  unsigned char *bytes;
  enum plm_buffer_mode mode;
} plm_buffer_t;

typedef struct {
  int16_t index;
  int16_t value;
} plm_vlc_t;

typedef struct {
  int16_t index;
  uint16_t value;
} plm_vlc_uint_t;

/* bool plm_buffer_has(plm_buffer_t *, size_t); */
/* int plm_buffer_read(plm_buffer_t *, int); */
/* void plm_buffer_align(plm_buffer_t *); */
/* void plm_buffer_skip(plm_buffer_t *, size_t); */
/* int plm_buffer_skip_bytes(plm_buffer_t *, unsigned char); */
/* int plm_buffer_next_start_code(plm_buffer_t *); */
/* int plm_buffer_find_start_code(plm_buffer_t *, int); */
/* int plm_buffer_no_start_code(plm_buffer_t *); */
/* int16_t plm_buffer_read_vlc(plm_buffer_t *, const plm_vlc_t *); */
/* uint16_t plm_buffer_read_vlc_uint(plm_buffer_t *, const plm_vlc_uint_t *); */

void plm_buffer_discard_read_bytes(plm_buffer_t *);
relegated void plm_buffer_load_file_callback(plm_buffer_t *, void *);

forceinline bool plm_buffer_has(plm_buffer_t *b, size_t bits) {
  unsigned have;
  have = b->length;
  have <<= 3;
  have -= b->bit_index;
  if (bits <= have) {
    return true;
  } else {
    if (b->load_callback) {
      b->load_callback(b, b->load_callback_user_data);
      return ((b->length << 3) - b->bit_index) >= bits;
    } else {
      return false;
    }
  }
}

forceinline int plm_buffer_read(plm_buffer_t *self, int count) {
  if (!plm_buffer_has(self, count)) return 0;
  int value = 0;
  while (count) {
    int current_byte = self->bytes[self->bit_index >> 3];
    int remaining = 8 - (self->bit_index & 7);         // Remaining bits in byte
    int read = remaining < count ? remaining : count;  // Bits in self run
    int shift = remaining - read;
    int mask = (0xff >> (8 - read));
    value = (value << read) | ((current_byte & (mask << shift)) >> shift);
    self->bit_index += read;
    count -= read;
  }
  return value;
}

forceinline void plm_buffer_align(plm_buffer_t *self) {
  self->bit_index = ((self->bit_index + 7) >> 3) << 3;
}

forceinline void plm_buffer_skip(plm_buffer_t *self, size_t count) {
  if (plm_buffer_has(self, count)) {
    self->bit_index += count;
  }
}

forceinline int plm_buffer_skip_bytes(plm_buffer_t *self, unsigned char v) {
  unsigned skipped;
  plm_buffer_align(self);
  skipped = 0;
  while (plm_buffer_has(self, 8)) {
    if (v == self->bytes[self->bit_index >> 3]) {
      self->bit_index += 8;
      ++skipped;
    } else {
      break;
    }
  }
  return skipped;
}

forceinline int plm_buffer_next_start_code(plm_buffer_t *self) {
  plm_buffer_align(self);
  while (plm_buffer_has(self, (5 << 3))) {
    size_t byte_index = (self->bit_index) >> 3;
    if (self->bytes[byte_index] == 0x00 &&
        self->bytes[byte_index + 1] == 0x00 &&
        self->bytes[byte_index + 2] == 0x01) {
      self->bit_index = (byte_index + 4) << 3;
      return self->bytes[byte_index + 3];
    }
    self->bit_index += 8;
  }
  self->bit_index = (self->length << 3);
  return -1;
}

forceinline int plm_buffer_find_start_code(plm_buffer_t *self, int code) {
  int current = 0;
  while (true) {
    current = plm_buffer_next_start_code(self);
    if (current == code || current == -1) {
      return current;
    }
  }
  return -1;
}

forceinline int plm_buffer_no_start_code(plm_buffer_t *self) {
  if (!plm_buffer_has(self, (5 << 3))) {
    return false;
  }
  size_t byte_index = ((self->bit_index + 7) >> 3);
  return !(self->bytes[byte_index] == 0x00 &&
           self->bytes[byte_index + 1] == 0x00 &&
           self->bytes[byte_index + 2] == 0x01);
}

forceinline int16_t plm_buffer_read_vlc(plm_buffer_t *self,
                                        const plm_vlc_t *table) {
  plm_vlc_t state = {0, 0};
  do {
    state = table[state.index + plm_buffer_read(self, 1)];
  } while (state.index > 0);
  return state.value;
}

forceinline uint16_t plm_buffer_read_vlc_uint(plm_buffer_t *self,
                                              const plm_vlc_uint_t *table) {
  return (uint16_t)plm_buffer_read_vlc(self, (plm_vlc_t *)table);
}

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_DSP_MPEG_BUFFER_H_ */
