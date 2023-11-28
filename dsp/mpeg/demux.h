#ifndef COSMOPOLITAN_DSP_MPEG_DEMUX_H_
#define COSMOPOLITAN_DSP_MPEG_DEMUX_H_
#include "dsp/mpeg/mpeg.h"
COSMOPOLITAN_C_START_

#define START_PACK 0xBA
#define START_END 0xB9
#define START_SYSTEM 0xBB

typedef struct plm_demux_t {
  plm_buffer_t *buffer;
  int destroy_buffer_when_done;
  double system_clock_ref;
  int has_pack_header;
  int has_system_header;
  int num_audio_streams;
  int num_video_streams;
  plm_packet_t current_packet;
  plm_packet_t next_packet;
} plm_demux_t;

double plm_demux_read_time(plm_demux_t *self);
void plm_demux_decode_pack_header(plm_demux_t *self);
void plm_demux_decode_system_header(plm_demux_t *self);
plm_packet_t *plm_demux_decode_packet(plm_demux_t *self, int start_code);
plm_packet_t *plm_demux_get_packet(plm_demux_t *self);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_DSP_MPEG_DEMUX_H_ */
