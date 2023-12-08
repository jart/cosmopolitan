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
#include "dsp/mpeg/mpeg.h"
#include "libc/log/log.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

asm(".ident\t\"\\n\\n\
PL_MPEG (MIT License)\\n\
Copyright(c) 2019 Dominic Szablewski\\n\
https://phoboslab.org\"");
asm(".include \"libc/disclaimer.inc\"");

/* clang-format off */
// -----------------------------------------------------------------------------
// plm (high-level interface) implementation

typedef struct plm_t {
	plm_demux_t *demux;
	double time;
	int has_ended;
	int loop;

	int video_packet_type;
	plm_buffer_t *video_buffer;
	plm_video_t *video_decoder;

	int audio_packet_type;
	double audio_lead_time;
	plm_buffer_t *audio_buffer;
	plm_audio_t *audio_decoder;

	plm_video_decode_callback video_decode_callback;
	void *video_decode_callback_user_data;

	plm_audio_decode_callback audio_decode_callback;
	void *audio_decode_callback_user_data;
} plm_t;

void plm_handle_end(plm_t *self);
void plm_read_video_packet(plm_buffer_t *buffer, void *user);
void plm_read_audio_packet(plm_buffer_t *buffer, void *user);
void plm_read_packets(plm_t *self, int requested_type);

plm_t *plm_create_with_filename(const char *filename) {
	plm_buffer_t *buffer = plm_buffer_create_with_filename(filename);
	if (!buffer) {
		return NULL;
	}
	return plm_create_with_buffer(buffer, true);
}

plm_t *plm_create_with_file(FILE *fh, int close_when_done) {
	plm_buffer_t *buffer = plm_buffer_create_with_file(fh, close_when_done);
	return plm_create_with_buffer(buffer, true);
}

plm_t *plm_create_with_memory(uint8_t *bytes, size_t length, int free_when_done) {
	plm_buffer_t *buffer = plm_buffer_create_with_memory(bytes, length, free_when_done);
	return plm_create_with_buffer(buffer, true);
}

plm_t *plm_create_with_buffer(plm_buffer_t *buffer, int destroy_when_done) {
	plm_t *self = (plm_t *)malloc(sizeof(plm_t));
	memset(self, 0, sizeof(plm_t));

	self->demux = plm_demux_create(buffer, destroy_when_done);

	// In theory we should check plm_demux_get_num_video_streams() and
	// plm_demux_get_num_audio_streams() here, but older files typically
	// do not specify these correctly. So we just assume we have a video and
	// audio stream and create the decoders.

	self->video_packet_type = PLM_DEMUX_PACKET_VIDEO_1;
	self->video_buffer = plm_buffer_create_with_capacity(PLM_BUFFER_DEFAULT_SIZE);
	plm_buffer_set_load_callback(self->video_buffer, plm_read_video_packet, self);

	self->audio_packet_type = PLM_DEMUX_PACKET_AUDIO_1;
	self->audio_buffer = plm_buffer_create_with_capacity(PLM_BUFFER_DEFAULT_SIZE);
	plm_buffer_set_load_callback(self->audio_buffer, plm_read_audio_packet, self);

	self->video_decoder = plm_video_create_with_buffer(self->video_buffer, true);
	self->audio_decoder = plm_audio_create_with_buffer(self->audio_buffer, true);

	return self;
}

void plm_destroy(plm_t *self) {
	plm_video_destroy(self->video_decoder);
	plm_audio_destroy(self->audio_decoder);
	plm_demux_destroy(self->demux);
	free(self);
}

int plm_get_audio_enabled(plm_t *self) {
	return (self->audio_packet_type != 0);
}

void plm_set_audio_enabled(plm_t *self, int enabled, int stream_index) {
	/* int num_streams = plm_demux_get_num_audio_streams(self->demux); */
	self->audio_packet_type = (enabled && stream_index >= 0 && stream_index < 4)
		? PLM_DEMUX_PACKET_AUDIO_1 + stream_index
		: 0;
}

int plm_get_video_enabled(plm_t *self) {
	return (self->video_packet_type != 0);
}

void plm_set_video_enabled(plm_t *self, int enabled) {
	self->video_packet_type = (enabled)
		? PLM_DEMUX_PACKET_VIDEO_1
		: 0;
}

int plm_get_width(plm_t *self) {
	return plm_video_get_width(self->video_decoder);
}

double plm_get_pixel_aspect_ratio(plm_t *self) {
	return plm_video_get_pixel_aspect_ratio(self->video_decoder);
}

int plm_get_height(plm_t *self) {
	return plm_video_get_height(self->video_decoder);
}

double plm_get_framerate(plm_t *self) {
	return plm_video_get_framerate(self->video_decoder);
}

int plm_get_num_audio_streams(plm_t *self) {
	// Some files do not specify the number of audio streams in the system header.
	// If the reported number of streams is 0, we check if we have a samplerate,
	// indicating at least one audio stream.
	int num_streams = plm_demux_get_num_audio_streams(self->demux);
	return num_streams == 0 && plm_get_samplerate(self) ? 1 : num_streams;
}

int plm_get_samplerate(plm_t *self) {
	return plm_audio_get_samplerate(self->audio_decoder);
}

double plm_get_audio_lead_time(plm_t *self) {
	return self->audio_lead_time;
}

void plm_set_audio_lead_time(plm_t *self, double lead_time) {
	self->audio_lead_time = lead_time;
}

double plm_get_time(plm_t *self) {
	return self->time;
}

void plm_rewind(plm_t *self) {
	plm_video_rewind(self->video_decoder);
	plm_audio_rewind(self->audio_decoder);
	plm_demux_rewind(self->demux);
	self->time = 0;
}

int plm_get_loop(plm_t *self) {
	return self->loop;
}

void plm_set_loop(plm_t *self, int loop) {
	self->loop = loop;
}

int plm_has_ended(plm_t *self) {
	return self->has_ended;
}

void plm_set_video_decode_callback(plm_t *self, plm_video_decode_callback fp, void *user) {
	self->video_decode_callback = fp;
	self->video_decode_callback_user_data = user;
}

void plm_set_audio_decode_callback(plm_t *self, plm_audio_decode_callback fp, void *user) {
	self->audio_decode_callback = fp;
	self->audio_decode_callback_user_data = user;
}

int plm_decode(plm_t *self, double tick) {
	DEBUGF("%s", "plm_decode");

	int decode_video = (self->video_decode_callback && self->video_packet_type);
	int decode_audio = (self->audio_decode_callback && self->audio_packet_type);

	if (!decode_video && !decode_audio) {
		// Nothing to do here
		return false;
	}

	int did_decode = false;
	int video_ended = false;
	int audio_ended = false;

	double video_target_time = self->time + tick;
	double audio_target_time = self->time + tick;

	if (self->audio_lead_time > 0 && decode_audio) {
		video_target_time -= self->audio_lead_time;
	}
	else {
		audio_target_time -= self->audio_lead_time;
	}

	do {
		did_decode = false;

		if (decode_video &&	plm_video_get_time(self->video_decoder) < video_target_time) {
			plm_frame_t *frame = plm_video_decode(self->video_decoder);
			if (frame) {
				self->video_decode_callback(self, frame, self->video_decode_callback_user_data);
				did_decode = true;
			}
			else {
				video_ended = true;
			}
		}

		if (decode_audio && plm_audio_get_time(self->audio_decoder) < audio_target_time) {
			plm_samples_t *samples = plm_audio_decode(self->audio_decoder);
			if (samples) {
				self->audio_decode_callback(self, samples, self->audio_decode_callback_user_data);
				did_decode = true;
			}
			else {
				audio_ended = true;
			}
		}
	} while (did_decode);

	// We wanted to decode something but failed -> the source must have ended
	if ((!decode_video || video_ended) && (!decode_audio || audio_ended)) {
		plm_handle_end(self);
	}
	else {
		self->time += tick;
	}

	return did_decode ? true : false;
}

plm_frame_t *plm_decode_video(plm_t *self) {
	if (!self->video_packet_type) {
		return NULL;
	}

	plm_frame_t *frame = plm_video_decode(self->video_decoder);
	if (frame) {
		self->time = frame->time;
	}
	else {
		plm_handle_end(self);
	}
	return frame;
}

plm_samples_t *plm_decode_audio(plm_t *self) {
	if (!self->audio_packet_type) {
		return NULL;
	}

	plm_samples_t *samples = plm_audio_decode(self->audio_decoder);
	if (samples) {
		self->time = samples->time;
	}
	else {
		plm_handle_end(self);
	}
	return samples;
}

void plm_handle_end(plm_t *self) {
	if (self->loop) {
		plm_rewind(self);
	}
	else {
		self->has_ended = true;
	}
}

void plm_read_video_packet(plm_buffer_t *buffer, void *user) {
	plm_t *self = (plm_t *)user;
	plm_read_packets(self, self->video_packet_type);
}

void plm_read_audio_packet(plm_buffer_t *buffer, void *user) {
	plm_t *self = (plm_t *)user;
	plm_read_packets(self, self->audio_packet_type);
}

void plm_read_packets(plm_t *self, int requested_type) {
	plm_packet_t *packet;
	while ((packet = plm_demux_decode(self->demux))) {
		if (packet->type == self->video_packet_type) {
			plm_buffer_write(self->video_buffer, packet->data, packet->length);
		}
		else if (packet->type == self->audio_packet_type) {
			plm_buffer_write(self->audio_buffer, packet->data, packet->length);
		}
		if (packet->type == requested_type) {
			return;
		}
	}
}
