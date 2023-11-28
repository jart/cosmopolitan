#ifndef COSMOPOLITAN_DSP_MPEG_VIDEO_H_
#define COSMOPOLITAN_DSP_MPEG_VIDEO_H_
#include "dsp/mpeg/mpeg.h"
COSMOPOLITAN_C_START_

typedef struct {
  int full_px;
  int is_set;
  int r_size;
  int h;
  int v;
} plm_video_motion_t;

typedef struct plm_video_t {
  double framerate;
  double time;
  double pixel_aspect_ratio;
  int frames_decoded;
  int width;
  int height;
  int mb_width;
  int mb_height;
  int mb_size;
  int luma_width;
  int luma_height;
  int chroma_width;
  int chroma_height;
  int start_code;
  int picture_type;
  plm_video_motion_t motion_forward;
  plm_video_motion_t motion_backward;
  int has_sequence_header;
  int quantizer_scale;
  int slice_begin;
  int macroblock_address;
  int mb_row;
  int mb_col;
  int macroblock_type;
  int macroblock_intra;
  int dc_predictor[3];
  plm_buffer_t *buffer;
  int destroy_buffer_when_done;
  plm_frame_t frame_current;
  plm_frame_t frame_forward;
  plm_frame_t frame_backward;
  uint8_t *frames_data;
  int block_data[64];
  uint8_t intra_quant_matrix[64];
  uint8_t non_intra_quant_matrix[64];
  int has_reference_frame;
  int assume_no_b_frames;
} plm_video_t;

void plm_video_process_macroblock_8(plm_video_t *, uint8_t *restrict,
                                    uint8_t *restrict, int, int, bool);
void plm_video_process_macroblock_16(plm_video_t *, uint8_t *restrict,
                                     uint8_t *restrict, int, int, bool);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_DSP_MPEG_VIDEO_H_ */
