#ifndef COSMOPOLITAN_DSP_MPEG_MPEG_H_
#define COSMOPOLITAN_DSP_MPEG_MPEG_H_
#include "libc/stdio/stdio.h"
COSMOPOLITAN_C_START_

typedef struct plm_t plm_t;
typedef struct plm_buffer_t plm_buffer_t;
typedef struct plm_demux_t plm_demux_t;
typedef struct plm_video_t plm_video_t;
typedef struct plm_audio_t plm_audio_t;

/**
 * Demuxed MPEG PS packet
 *
 * The type maps directly to the various MPEG-PES start codes. pts is
 * the presentation time stamp of the packet in seconds. Not all packets
 * have a pts value.
 */
typedef struct plm_packet_t {
  int type;
  double pts;
  size_t length;
  uint8_t *data;
} plm_packet_t;

/**
 * Decoded Video Plane
 *
 * The byte length of the data is width * height. Note that different
 * planes have different sizes: the Luma plane (Y) is double the size of
 * each of the two Chroma planes (Cr, Cb) - i.e. 4 times the byte
 * length. Also note that the size of the plane does *not* denote the
 * size of the displayed frame. The sizes of planes are always rounded
 * up to the nearest macroblock (16px).
 */
typedef struct plm_plane_t {
  unsigned int width;
  unsigned int height;
  uint8_t *data;
} plm_plane_t;

/**
 * Decoded Video Frame
 *
 * Width and height denote the desired display size of the frame. This
 * may be different from the internal size of the 3 planes.
 */
typedef struct plm_frame_t {
  double time;
  unsigned int width;
  unsigned int height;
  plm_plane_t y;
  plm_plane_t cr;
  plm_plane_t cb;
} plm_frame_t;

/**
 * Callback function type for decoded video frames used by the high-level
 * plm_* interface
 */
typedef void (*plm_video_decode_callback)(plm_t *self, plm_frame_t *frame,
                                          void *user);

/**
 * Decoded Audio Samples
 *
 * Samples are stored as normalized (-1, 1) float either interleaved, or if
 * PLM_AUDIO_SEPARATE_CHANNELS is defined, in two separate arrays.
 * The `count` is always PLM_AUDIO_SAMPLES_PER_FRAME and just there for
 * convenience.
 */
#define PLM_AUDIO_SAMPLES_PER_FRAME 1152

struct plm_samples_t {
  double time;
  unsigned int count;
#ifdef PLM_AUDIO_SEPARATE_CHANNELS
  float left[PLM_AUDIO_SAMPLES_PER_FRAME] forcealign(32);
  float right[PLM_AUDIO_SAMPLES_PER_FRAME] forcealign(32);
#else
  float interleaved[PLM_AUDIO_SAMPLES_PER_FRAME * 2] forcealign(32);
#endif
} forcealign(32);

typedef struct plm_samples_t plm_samples_t;

/**
 * Callback function type for decoded audio samples used by the high-level
 * plm_* interface
 */
typedef void (*plm_audio_decode_callback)(plm_t *self, plm_samples_t *samples,
                                          void *user);

/**
 * Callback function for plm_buffer when it needs more data
 */
typedef void (*plm_buffer_load_callback)(plm_buffer_t *self, void *user);

/**
 * -----------------------------------------------------------------------------
 * plm_* public API
 * High-Level API for loading/demuxing/decoding MPEG-PS data
 *
 * Create a plmpeg instance with a filename. Returns NULL if the file could not
 * be opened.
 */
plm_t *plm_create_with_filename(const char *filename);

/**
 * Create a plmpeg instance with file handle. Pass true to close_when_done
 * to let plmpeg call fclose() on the handle when plm_destroy() is
 * called.
 */
plm_t *plm_create_with_file(FILE *fh, int close_when_done);

/**
 * Create a plmpeg instance with pointer to memory as source. This assumes the
 * whole file is in memory. Pass true to free_when_done to let plmpeg call
 * free() on the pointer when plm_destroy() is called.
 */
plm_t *plm_create_with_memory(uint8_t *bytes, size_t length,
                              int free_when_done);

/**
 * Create a plmpeg instance with a plm_buffer as source. This is also
 * called internally by all the above constructor functions.
 */
plm_t *plm_create_with_buffer(plm_buffer_t *buffer, int destroy_when_done);

/**
 * Destroy a plmpeg instance and free all data
 */
void plm_destroy(plm_t *self);

/**
 * Get or set whether video decoding is enabled.
 */
int plm_get_video_enabled(plm_t *self);
void plm_set_video_enabled(plm_t *self, int enabled);

/**
 * Get or set whether audio decoding is enabled. When enabling, you can set the
 * desired audio stream (0-3) to decode.
 */
int plm_get_audio_enabled(plm_t *self);
void plm_set_audio_enabled(plm_t *self, int enabled, int stream_index);

/**
 * Get the display width/height of the video stream
 */
int plm_get_width(plm_t *self);
int plm_get_height(plm_t *self);

double plm_get_pixel_aspect_ratio(plm_t *);

/**
 * Get the framerate of the video stream in frames per second
 */
double plm_get_framerate(plm_t *self);

/**
 * Get the number of available audio streams in the file
 */
int plm_get_num_audio_streams(plm_t *self);

/**
 * Get the samplerate of the audio stream in samples per second
 */
int plm_get_samplerate(plm_t *self);

/**
 * Get or set the audio lead time in seconds - the time in which audio samples
 * are decoded in advance (or behind) the video decode time. Default 0.
 */
double plm_get_audio_lead_time(plm_t *self);
void plm_set_audio_lead_time(plm_t *self, double lead_time);

/**
 * Get the current internal time in seconds
 */
double plm_get_time(plm_t *self);

/**
 * Rewind all buffers back to the beginning.
 */
void plm_rewind(plm_t *self);

/**
 * Get or set looping. Default false.
 */
int plm_get_loop(plm_t *self);
void plm_set_loop(plm_t *self, int loop);

/**
 * Get whether the file has ended. If looping is enabled, this will always
 * return false.
 */
int plm_has_ended(plm_t *self);

/**
 * Set the callback for decoded video frames used with plm_decode(). If no
 * callback is set, video data will be ignored and not be decoded.
 */
void plm_set_video_decode_callback(plm_t *self, plm_video_decode_callback fp,
                                   void *user);

/**
 * Set the callback for decoded audio samples used with plm_decode(). If no
 * callback is set, audio data will be ignored and not be decoded.
 */
void plm_set_audio_decode_callback(plm_t *self, plm_audio_decode_callback fp,
                                   void *user);

/**
 * Advance the internal timer by seconds and decode video/audio up to
 * this time. Returns true/false whether anything was decoded.
 */
int plm_decode(plm_t *self, double seconds);

/**
 * Decode and return one video frame. Returns NULL if no frame could be decoded
 * (either because the source ended or data is corrupt). If you only want to
 * decode video, you should disable audio via plm_set_audio_enabled().
 * The returned plm_frame_t is valid until the next call to
 * plm_decode_video call or until the plm_destroy is called.
 */
plm_frame_t *plm_decode_video(plm_t *self);

/**
 * Decode and return one audio frame. Returns NULL if no frame could be decoded
 * (either because the source ended or data is corrupt). If you only want to
 * decode audio, you should disable video via plm_set_video_enabled().
 * The returned plm_samples_t is valid until the next call to
 * plm_decode_video or until the plm_destroy is called.
 */
plm_samples_t *plm_decode_audio(plm_t *self);

/* -----------------------------------------------------------------------------
 * plm_buffer public API
 * Provides the data source for all other plm_* interfaces
 *
 * The default size for buffers created from files or by the high-level API
 */
#ifndef PLM_BUFFER_DEFAULT_SIZE
#define PLM_BUFFER_DEFAULT_SIZE (128 * 1024)
#endif

/**
 * Create a buffer instance with a filename. Returns NULL if the file could not
 * be opened.
 */
plm_buffer_t *plm_buffer_create_with_filename(const char *filename);

/**
 * Create a buffer instance with file handle. Pass true to close_when_done
 * to let plmpeg call fclose() on the handle when plm_destroy() is
 * called.
 */
plm_buffer_t *plm_buffer_create_with_file(FILE *fh, int close_when_done);

/**
 * Create a buffer instance with a pointer to memory as source. This assumes
 * the whole file is in memory. Pass 1 to free_when_done to let plmpeg call
 * free() on the pointer when plm_destroy() is called.
 */
plm_buffer_t *plm_buffer_create_with_memory(uint8_t *bytes, size_t length,
                                            int free_when_done);

/**
 * Create an empty buffer with an initial capacity. The buffer will grow
 * as needed.
 */
plm_buffer_t *plm_buffer_create_with_capacity(size_t capacity);

/**
 * Destroy a buffer instance and free all data
 */
void plm_buffer_destroy(plm_buffer_t *self);

/**
 * Copy data into the buffer. If the data to be written is larger than the
 * available space, the buffer will realloc() with a larger capacity.
 * Returns the number of bytes written. This will always be the same as the
 * passed in length, except when the buffer was created _with_memory() for
 * which _write() is forbidden.
 */
size_t plm_buffer_write(plm_buffer_t *self, uint8_t *bytes, size_t length);

/**
 * Set a callback that is called whenever the buffer needs more data
 */
void plm_buffer_set_load_callback(plm_buffer_t *self,
                                  plm_buffer_load_callback fp, void *user);

/**
 * Rewind the buffer back to the beginning. When loading from a file handle,
 * this also seeks to the beginning of the file.
 */
void plm_buffer_rewind(plm_buffer_t *self);

/**
 * -----------------------------------------------------------------------------
 * plm_demux public API
 * Demux an MPEG Program Stream (PS) data into separate packages
 *
 * Various Packet Types
 */
#define PLM_DEMUX_PACKET_PRIVATE 0xBD
#define PLM_DEMUX_PACKET_AUDIO_1 0xC0
#define PLM_DEMUX_PACKET_AUDIO_2 0xC1
#define PLM_DEMUX_PACKET_AUDIO_3 0xC2
#define PLM_DEMUX_PACKET_AUDIO_4 0xC2
#define PLM_DEMUX_PACKET_VIDEO_1 0xE0

/**
 * Create a demuxer with a plm_buffer as source
 */
plm_demux_t *plm_demux_create(plm_buffer_t *buffer, int destroy_when_done);

/**
 * Destroy a demuxer and free all data
 */
void plm_demux_destroy(plm_demux_t *self);

/**
 * Returns the number of video streams found in the system header.
 */
int plm_demux_get_num_video_streams(plm_demux_t *self);

/**
 * Returns the number of audio streams found in the system header.
 */
int plm_demux_get_num_audio_streams(plm_demux_t *self);

/**
 * Rewinds the internal buffer. See plm_buffer_rewind().
 */
void plm_demux_rewind(plm_demux_t *self);

/**
 * Decode and return the next packet. The returned packet_t is valid until
 * the next call to plm_demux_decode() or until the demuxer is destroyed.
 */
plm_packet_t *plm_demux_decode(plm_demux_t *self);

/* -----------------------------------------------------------------------------
 * plm_video public API
 * Decode MPEG1 Video ("mpeg1") data into raw YCrCb frames
 */

/**
 * Create a video decoder with a plm_buffer as source
 */
plm_video_t *plm_video_create_with_buffer(plm_buffer_t *buffer,
                                          int destroy_when_done);

/**
 * Destroy a video decoder and free all data
 */
void plm_video_destroy(plm_video_t *self);

/**
 * Get the framerate in frames per second
 */
double plm_video_get_framerate(plm_video_t *);

double plm_video_get_pixel_aspect_ratio(plm_video_t *);

/**
 * Get the display width/height
 */
int plm_video_get_width(plm_video_t *);
int plm_video_get_height(plm_video_t *);

/**
 * Set "no delay" mode. When enabled, the decoder assumes that the video does
 * *not* contain any B-Frames. This is useful for reducing lag when streaming.
 */
void plm_video_set_no_delay(plm_video_t *self, int no_delay);

/**
 * Get the current internal time in seconds
 */
double plm_video_get_time(plm_video_t *self);

/**
 * Rewinds the internal buffer. See plm_buffer_rewind().
 */
void plm_video_rewind(plm_video_t *self);

/**
 * Decode and return one frame of video and advance the internal time by
 * 1/framerate seconds. The returned frame_t is valid until the next call of
 * plm_video_decode() or until the video decoder is destroyed.
 */
plm_frame_t *plm_video_decode(plm_video_t *self);

/**
 * Convert the YCrCb data of a frame into an interleaved RGB buffer. The buffer
 * pointed to by *rgb must have a size of (frame->width * frame->height * 3)
 * bytes.
 */
void plm_frame_to_rgb(plm_frame_t *frame, uint8_t *rgb);

/* -----------------------------------------------------------------------------
 * plm_audio public API
 * Decode MPEG-1 Audio Layer II ("mp2") data into raw samples
 */

/**
 * Create an audio decoder with a plm_buffer as source.
 */
plm_audio_t *plm_audio_create_with_buffer(plm_buffer_t *buffer,
                                          int destroy_when_done);

/**
 * Destroy an audio decoder and free all data
 */
void plm_audio_destroy(plm_audio_t *self);

/**
 * Get the samplerate in samples per second
 */
int plm_audio_get_samplerate(plm_audio_t *self);

/**
 * Get the current internal time in seconds
 */
double plm_audio_get_time(plm_audio_t *self);

/**
 * Rewinds the internal buffer. See plm_buffer_rewind().
 */
void plm_audio_rewind(plm_audio_t *self);

/**
 * Decode and return one "frame" of audio and advance the internal time by
 * (PLM_AUDIO_SAMPLES_PER_FRAME/samplerate) seconds. The returned samples_t
 * is valid until the next call of plm_audio_decode() or until the audio
 * decoder is destroyed.
 */
plm_samples_t *plm_audio_decode(plm_audio_t *self);

extern long plmpegdecode_latency_;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_DSP_MPEG_MPEG_H_ */
