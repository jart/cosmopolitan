/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:4;tab-width:4;coding:utf-8   -*-│
│ vi: set noet ft=c ts=4 sw=4 fenc=utf-8                                   :vi │
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
#include "dsp/mpeg/buffer.h"
#include "dsp/mpeg/mpeg.h"
#include "libc/calls/calls.h"
#include "libc/log/check.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/madv.h"

asm(".ident\t\"\\n\\n\
PL_MPEG (MIT License)\\n\
Copyright(c) 2019 Dominic Szablewski\\n\
https://phoboslab.org\"");
asm(".include \"libc/disclaimer.inc\"");

/* clang-format off */
// -----------------------------------------------------------------------------
// plm_buffer implementation

plm_buffer_t *plm_buffer_create_with_filename(const char *filename) {
	FILE *fh = fopen(filename, "rb");
	if (!fh) {
		return NULL;
	}
	fadvise(fileno(fh), 0, 0, MADV_SEQUENTIAL);
	return plm_buffer_create_with_file(fh, true);
}

plm_buffer_t *plm_buffer_create_with_file(FILE *fh, int close_when_done) {
	plm_buffer_t *b;
	b = plm_buffer_create_with_capacity(PLM_BUFFER_DEFAULT_SIZE);
	b->fh = fh;
	b->close_when_done = close_when_done;
	b->mode = PLM_BUFFER_MODE_FILE;
	plm_buffer_set_load_callback(b, plm_buffer_load_file_callback, NULL);
	return b;
}

plm_buffer_t *plm_buffer_create_with_memory(unsigned char *bytes, size_t length, int free_when_done) {
	plm_buffer_t *b;
	b = memalign(_Alignof(plm_buffer_t), sizeof(plm_buffer_t));
	memset(b, 0, sizeof(plm_buffer_t));
	b->capacity = length;
	b->length = length;
	b->free_when_done = free_when_done;
	b->bytes = bytes;
	b->mode = PLM_BUFFER_MODE_FIXED_MEM;
	return b;
}

plm_buffer_t * plm_buffer_create_with_capacity(size_t capacity) {
	plm_buffer_t *b;
	b = memalign(_Alignof(plm_buffer_t), sizeof(plm_buffer_t));
	memset(b, 0, sizeof(plm_buffer_t));
	b->capacity = capacity;
	b->free_when_done = true;
	b->bytes = (unsigned char *)malloc(capacity);
	b->mode = PLM_BUFFER_MODE_DYNAMIC_MEM;
	return b;
}

void plm_buffer_destroy(plm_buffer_t *self) {
	if (self->fh && self->close_when_done) {
		fclose(self->fh);
	}
	if (self->free_when_done) {
		free(self->bytes);
	}
	free(self);
}

size_t plm_buffer_write(plm_buffer_t *self, unsigned char *bytes, size_t length) {
	if (self->mode == PLM_BUFFER_MODE_FIXED_MEM) {
		return 0;
	}
	// This should be a ring buffer, but instead it just shifts all unread data
	// to the beginning of the buffer and appends new data at the end. Seems
	// to be good enough.
	plm_buffer_discard_read_bytes(self);
	// Do we have to resize to fit the new data?
	size_t bytes_available = self->capacity - self->length;
	if (bytes_available < length) {
		size_t new_size = self->capacity;
		do {
			new_size *= 2;
		} while (new_size - self->length < length);
		self->bytes = (unsigned char *)realloc(self->bytes, new_size);
		self->capacity = new_size;
	}
	memcpy(self->bytes + self->length, bytes, length);
	self->length += length;
	return length;
}

void plm_buffer_set_load_callback(plm_buffer_t *self, plm_buffer_load_callback fp, void *user) {
	self->load_callback = fp;
	self->load_callback_user_data = user;
}

void plm_buffer_rewind(plm_buffer_t *self) {
	if (self->fh) {
		fseek(self->fh, 0, SEEK_SET);
		self->length = 0;
	}
	if (self->mode != PLM_BUFFER_MODE_FIXED_MEM) {
		self->length = 0;
	}
	self->bit_index = 0;
}

void plm_buffer_discard_read_bytes(plm_buffer_t *self) {
	size_t byte_pos = self->bit_index >> 3;
	if (byte_pos == self->length) {
		self->bit_index = 0;
		self->length = 0;
	}
	else if (byte_pos > 0) {
		memmove(self->bytes, self->bytes + byte_pos, self->length - byte_pos);
		self->bit_index -= byte_pos << 3;
		self->length -= byte_pos;
	}
}

void plm_buffer_load_file_callback(plm_buffer_t *self, void *user) {
	plm_buffer_discard_read_bytes(self);
	unsigned bytes_available = self->capacity - self->length;
	unsigned bytes_read = fread(self->bytes + self->length, 1, bytes_available, self->fh);
	self->length += bytes_read;
}
