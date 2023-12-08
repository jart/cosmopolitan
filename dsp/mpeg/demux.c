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
#include "dsp/mpeg/demux.h"
#include "dsp/mpeg/mpeg.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"

asm(".ident\t\"\\n\\n\
PL_MPEG (MIT License)\\n\
Copyright(c) 2019 Dominic Szablewski\\n\
https://phoboslab.org\"");
asm(".include \"libc/disclaimer.inc\"");

/* clang-format off */
// ----------------------------------------------------------------------------
// plm_demux implementation

plm_demux_t *plm_demux_create(plm_buffer_t *buffer, int destroy_when_done) {
	plm_demux_t *self = (plm_demux_t *)malloc(sizeof(plm_demux_t));
	memset(self, 0, sizeof(plm_demux_t));

	self->buffer = buffer;
	self->destroy_buffer_when_done = destroy_when_done;

	if (plm_buffer_find_start_code(self->buffer, START_PACK) != -1) {
		plm_demux_decode_pack_header(self);
	}
	if (plm_buffer_find_start_code(self->buffer, START_SYSTEM) != -1) {
		plm_demux_decode_system_header(self);
	}
	return self;
}

void plm_demux_destroy(plm_demux_t *self) {
	if (self->destroy_buffer_when_done) {
		plm_buffer_destroy(self->buffer);
	}
	free(self);
}

int plm_demux_get_num_video_streams(plm_demux_t *self) {
	return self->num_video_streams;
}

int plm_demux_get_num_audio_streams(plm_demux_t *self) {
	return self->num_audio_streams;
}

void plm_demux_rewind(plm_demux_t *self) {
	plm_buffer_rewind(self->buffer);
}

plm_packet_t *plm_demux_decode(plm_demux_t *self) {
	if (self->current_packet.length) {
		size_t bits_till_next_packet = self->current_packet.length << 3;
		if (!plm_buffer_has(self->buffer, bits_till_next_packet)) {
			return NULL;
		}
		plm_buffer_skip(self->buffer, bits_till_next_packet);
		self->current_packet.length = 0;
	}

	if (!self->has_pack_header) {
		if (plm_buffer_find_start_code(self->buffer, START_PACK) != -1) {
			plm_demux_decode_pack_header(self);
		}
		else {
			return NULL;
		}
	}

	if (!self->has_system_header) {
		if (plm_buffer_find_start_code(self->buffer, START_SYSTEM) != -1) {
			plm_demux_decode_system_header(self);
		}
		else {
			return NULL;
		}
	}

	// pending packet just waiting for data?
	if (self->next_packet.length) {
		return plm_demux_get_packet(self);
	}

	int code;
	do {
		code = plm_buffer_next_start_code(self->buffer);
		if (
			code == PLM_DEMUX_PACKET_VIDEO_1 ||
			code == PLM_DEMUX_PACKET_PRIVATE ||
			(code >= PLM_DEMUX_PACKET_AUDIO_1 && code <= PLM_DEMUX_PACKET_AUDIO_4)
		) {
			return plm_demux_decode_packet(self, code);
		}
	} while (code != -1);

	return NULL;
}

double plm_demux_read_time(plm_demux_t *self) {
	int64_t clock = plm_buffer_read(self->buffer, 3) << 30;
	plm_buffer_skip(self->buffer, 1);
	clock |= plm_buffer_read(self->buffer, 15) << 15;
	plm_buffer_skip(self->buffer, 1);
	clock |= plm_buffer_read(self->buffer, 15);
	plm_buffer_skip(self->buffer, 1);
	return (double)clock / 90000.0;
}

void plm_demux_decode_pack_header(plm_demux_t *self) {
	if (plm_buffer_read(self->buffer, 4) != 0x02) {
		return; // invalid
	}
	self->system_clock_ref = plm_demux_read_time(self);
	plm_buffer_skip(self->buffer, 1);
	plm_buffer_skip(self->buffer, 22); // mux_rate * 50
	plm_buffer_skip(self->buffer, 1);

	self->has_pack_header = true;
}

void plm_demux_decode_system_header(plm_demux_t *self) {
	plm_buffer_skip(self->buffer, 16); // header_length
	plm_buffer_skip(self->buffer, 24); // rate bound
	self->num_audio_streams = plm_buffer_read(self->buffer, 6);
	plm_buffer_skip(self->buffer, 5); // misc flags
	self->num_video_streams = plm_buffer_read(self->buffer, 5);

	self->has_system_header = true;
}

plm_packet_t *plm_demux_decode_packet(plm_demux_t *self, int start_code) {
	if (!plm_buffer_has(self->buffer, 8 << 3)) {
		return NULL;
	}

	self->next_packet.type = start_code;
	self->next_packet.length = plm_buffer_read(self->buffer, 16);
	self->next_packet.length -= plm_buffer_skip_bytes(self->buffer, 0xff); // stuffing

	// skip P-STD
	if (plm_buffer_read(self->buffer, 2) == 0x01) {
		plm_buffer_skip(self->buffer, 16);
		self->next_packet.length -= 2;
	}

	int pts_dts_marker = plm_buffer_read(self->buffer, 2);
	if (pts_dts_marker == 0x03) {
		self->next_packet.pts = plm_demux_read_time(self);
		plm_buffer_skip(self->buffer, 40); // skip dts
		self->next_packet.length -= 10;
	}
	else if (pts_dts_marker == 0x02) {
		self->next_packet.pts = plm_demux_read_time(self);
		self->next_packet.length -= 5;
	}
	else if (pts_dts_marker == 0x00) {
		self->next_packet.pts = 0;
		plm_buffer_skip(self->buffer, 4);
		self->next_packet.length -= 1;
	}
	else {
		return NULL; // invalid
	}

	return plm_demux_get_packet(self);
}

plm_packet_t *plm_demux_get_packet(plm_demux_t *self) {
	if (!plm_buffer_has(self->buffer, self->next_packet.length << 3)) {
		return NULL;
	}
	self->current_packet.data = self->buffer->bytes + (self->buffer->bit_index >> 3);
	self->current_packet.length = self->next_packet.length;
	self->current_packet.type = self->next_packet.type;
	self->current_packet.pts = self->next_packet.pts;
	self->next_packet.length = 0;
	return &self->current_packet;
}
