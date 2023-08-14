#ifndef COSMOPOLITAN_THIRD_PARTY_STB_STB_VORBIS_H_
#define COSMOPOLITAN_THIRD_PARTY_STB_STB_VORBIS_H_
#include "libc/stdio/stdio.h"
COSMOPOLITAN_C_START_

enum STBVorbisError {
  VORBIS__no_error,
  VORBIS_need_more_data = 1,     // not a real error
  VORBIS_invalid_api_mixing,     // can't mix API modes
  VORBIS_outofmem,               // not enough memory
  VORBIS_feature_not_supported,  // uses floor 0
  VORBIS_too_many_channels,      // STB_VORBIS_MAX_CHANNELS is too small
  VORBIS_file_open_failure,      // fopen() failed
  VORBIS_seek_without_length,    // can't seek in unknown-length file
  VORBIS_unexpected_eof = 10,    // file is truncated?
  VORBIS_seek_invalid,           // seek past EOF
  VORBIS_invalid_setup = 20,     // decoding errors
  VORBIS_invalid_stream,
  VORBIS_missing_capture_pattern = 30,  // ogg errors
  VORBIS_invalid_stream_structure_version,
  VORBIS_continued_packet_flag_invalid,
  VORBIS_incorrect_stream_serial_number,
  VORBIS_invalid_first_page,
  VORBIS_bad_packet_type,
  VORBIS_cant_find_last_page,
  VORBIS_seek_failed,
  VORBIS_ogg_skeleton_not_supported
};

typedef struct {
  char *alloc_buffer;
  int alloc_buffer_length_in_bytes;
} stb_vorbis_alloc;

typedef struct stb_vorbis stb_vorbis;

typedef struct {
  unsigned int sample_rate;
  int channels;
  unsigned int setup_memory_required;
  unsigned int setup_temp_memory_required;
  unsigned int temp_memory_required;
  int max_frame_size;
} stb_vorbis_info;

typedef struct {
  char *vendor;
  int comment_list_length;
  char **comment_list;
} stb_vorbis_comment;

// get general information about the file
stb_vorbis_info stb_vorbis_get_info(stb_vorbis *f);

// get ogg comments
stb_vorbis_comment stb_vorbis_get_comment(stb_vorbis *f);

// get the last error detected (clears it, too)
int stb_vorbis_get_error(stb_vorbis *f);

// close an ogg vorbis file and free all memory in use
void stb_vorbis_close(stb_vorbis *f);

// this function returns the offset (in samples) from the beginning of the
// file that will be returned by the next decode, if it is known, or -1
// otherwise. after a flush_pushdata() call, this may take a while before
// it becomes valid again.
// NOT WORKING YET after a seek with PULLDATA API
int stb_vorbis_get_sample_offset(stb_vorbis *f);

// returns the current seek point within the file, or offset from the beginning
// of the memory buffer. In pushdata mode it returns 0.
unsigned int stb_vorbis_get_file_offset(stb_vorbis *f);

////////////////////////////////////////////////////////////////////////////////
// PUSHDATA

// this API allows you to get blocks of data from any source and hand
// them to stb_vorbis. you have to buffer them; stb_vorbis will tell
// you how much it used, and you have to give it the rest next time;
// and stb_vorbis may not have enough data to work with and you will
// need to give it the same data again PLUS more. Note that the Vorbis
// specification does not bound the size of an individual frame.

stb_vorbis *stb_vorbis_open_pushdata(const unsigned char *datablock,
                                     int datablock_length_in_bytes,
                                     int *datablock_memory_consumed_in_bytes,
                                     int *error,
                                     const stb_vorbis_alloc *alloc_buffer);
// create a vorbis decoder by passing in the initial data block containing
//    the ogg&vorbis headers (you don't need to do parse them, just provide
//    the first N bytes of the file--you're told if it's not enough, see below)
// on success, returns an stb_vorbis *, does not set error, returns the amount
// of
//    data parsed/consumed on this call in *datablock_memory_consumed_in_bytes;
// on failure, returns NULL on error and sets *error, does not change
// *datablock_memory_consumed if returns NULL and *error is
// VORBIS_need_more_data, then the input block was
//       incomplete and you need to pass in a larger block from the start of the
//       file

int stb_vorbis_decode_frame_pushdata(
    stb_vorbis *f, const unsigned char *datablock,
    int datablock_length_in_bytes,
    int *channels,    // place to write number of float * buffers
    float ***output,  // place to write float ** array of float * buffers
    int *samples      // place to write number of output samples
);
// decode a frame of audio sample data if possible from the passed-in data block
//
// return value: number of bytes we used from datablock
//
// possible cases:
//     0 bytes used, 0 samples output (need more data)
//     N bytes used, 0 samples output (resynching the stream, keep going)
//     N bytes used, M samples output (one frame of data)
// note that after opening a file, you will ALWAYS get one N-bytes,0-sample
// frame, because Vorbis always "discards" the first frame.
//
// Note that on resynch, stb_vorbis will rarely consume all of the buffer,
// instead only datablock_length_in_bytes-3 or less. This is because it wants
// to avoid missing parts of a page header if they cross a datablock boundary,
// without writing state-machiney code to record a partial detection.
//
// The number of channels returned are stored in *channels (which can be
// NULL--it is always the same as the number of channels reported by
// get_info). *output will contain an array of float* buffers, one per
// channel. In other words, (*output)[0][0] contains the first sample from
// the first channel, and (*output)[1][0] contains the first sample from
// the second channel.
//
// *output points into stb_vorbis's internal output buffer storage; these
// buffers are owned by stb_vorbis and application code should not free
// them or modify their contents. They are transient and will be overwritten
// once you ask for more data to get decoded, so be sure to grab any data
// you need before then.

void stb_vorbis_flush_pushdata(stb_vorbis *f);
// inform stb_vorbis that your next datablock will not be contiguous with
// previous ones (e.g. you've seeked in the data); future attempts to decode
// frames will cause stb_vorbis to resynchronize (as noted above), and
// once it sees a valid Ogg page (typically 4-8KB, as large as 64KB), it
// will begin decoding the _next_ frame.
//
// if you want to seek using pushdata, you need to seek in your file, then
// call stb_vorbis_flush_pushdata(), then start calling decoding, then once
// decoding is returning you data, call stb_vorbis_get_sample_offset, and
// if you don't like the result, seek your file again and repeat.

////////////////////////////////////////////////////////////////////////////////
// PULLING INPUT API
//
// This API assumes stb_vorbis is allowed to pull data from a source--
// either a block of memory containing the _entire_ vorbis stream, or a
// FILE * that you or it create, or possibly some other reading mechanism
// if you go modify the source to replace the FILE * case with some kind
// of callback to your code. (But if you don't support seeking, you may
// just want to go ahead and use pushdata.)

int stb_vorbis_decode_filename(const char *filename, int *channels,
                               int *sample_rate, short **output);

// decode an entire file and output the data interleaved into a malloc()ed
// buffer stored in *output. The return value is the number of samples
// decoded, or -1 if the file could not be opened or was not an ogg vorbis file.
// When you're done with it, just free() the pointer returned in *output.
int stb_vorbis_decode_memory(const unsigned char *mem, int len, int *channels,
                             int *sample_rate, short **output);

// create an ogg vorbis decoder from an ogg vorbis stream in memory (note
// this must be the entire stream!). on failure, returns NULL and sets *error
stb_vorbis *stb_vorbis_open_memory(const unsigned char *data, int len,
                                   int *error,
                                   const stb_vorbis_alloc *alloc_buffer);

// create an ogg vorbis decoder from a filename via fopen(). on failure,
// returns NULL and sets *error (possibly to VORBIS_file_open_failure).
stb_vorbis *stb_vorbis_open_filename(const char *filename, int *error,
                                     const stb_vorbis_alloc *alloc_buffer);

// create an ogg vorbis decoder from an open FILE *, looking for a stream at
// the _current_ seek point (ftell). on failure, returns NULL and sets *error.
// note that stb_vorbis must "own" this stream; if you seek it in between
// calls to stb_vorbis, it will become confused. Moreover, if you attempt to
// perform stb_vorbis_seek_*() operations on this file, it will assume it
// owns the _entire_ rest of the file after the start point. Use the next
// function, stb_vorbis_open_file_section(), to limit it.
stb_vorbis *stb_vorbis_open_file(FILE *f, int close_handle_on_close, int *error,
                                 const stb_vorbis_alloc *alloc_buffer);

// create an ogg vorbis decoder from an open FILE *, looking for a stream at
// the _current_ seek point (ftell); the stream will be of length 'len' bytes.
// on failure, returns NULL and sets *error. note that stb_vorbis must "own"
// this stream; if you seek it in between calls to stb_vorbis, it will become
// confused.
stb_vorbis *stb_vorbis_open_file_section(FILE *f, int close_handle_on_close,
                                         int *error,
                                         const stb_vorbis_alloc *alloc_buffer,
                                         unsigned int len);

// these functions seek in the Vorbis file to (approximately) 'sample_number'.
// after calling seek_frame(), the next call to get_frame_*() will include
// the specified sample. after calling stb_vorbis_seek(), the next call to
// stb_vorbis_get_samples_* will start with the specified sample. If you
// do not need to seek to EXACTLY the target sample when using get_samples_*,
// you can also use seek_frame().
int stb_vorbis_seek_frame(stb_vorbis *f, unsigned int sample_number);
int stb_vorbis_seek(stb_vorbis *f, unsigned int sample_number);

// this function is equivalent to stb_vorbis_seek(f,0)
int stb_vorbis_seek_start(stb_vorbis *f);

// these functions return the total length of the vorbis stream
unsigned int stb_vorbis_stream_length_in_samples(stb_vorbis *f);
float stb_vorbis_stream_length_in_seconds(stb_vorbis *f);

// decode the next frame and return the number of samples. the number of
// channels returned are stored in *channels (which can be NULL--it is always
// the same as the number of channels reported by get_info). *output will
// contain an array of float* buffers, one per channel. These outputs will
// be overwritten on the next call to stb_vorbis_get_frame_*.
//
// You generally should not intermix calls to stb_vorbis_get_frame_*()
// and stb_vorbis_get_samples_*(), since the latter calls the former.
int stb_vorbis_get_frame_float(stb_vorbis *f, int *channels, float ***output);

int stb_vorbis_get_frame_short_interleaved(stb_vorbis *f, int num_c,
                                           short *buffer, int num_shorts);
// decode the next frame and return the number of *samples* per channel.
// Note that for interleaved data, you pass in the number of shorts (the
// size of your array), but the return value is the number of samples per
// channel, not the total number of samples.
//
// The data is coerced to the number of channels you request according to the
// channel coercion rules (see below). You must pass in the size of your
// buffer(s) so that stb_vorbis will not overwrite the end of the buffer.
// The maximum buffer size needed can be gotten from get_info(); however,
// the Vorbis I specification implies an absolute maximum of 4096 samples
// per channel.
int stb_vorbis_get_frame_short(stb_vorbis *f, int num_c, short **buffer,
                               int num_samples);

// Channel coercion rules:
//    Let M be the number of channels requested, and N the number of channels
//    present, and Cn be the nth channel; let stereo L be the sum of all L and
//    center channels, and stereo R be the sum of all R and center channels
//    (channel assignment from the vorbis spec).
//        M    N       output
//        1    k      sum(Ck) for all k
//        2    *      stereo L, stereo R
//        k    l      k > l, the first l channels, then 0s
//        k    l      k <= l, the first k channels
//    Note that this is not _good_ surround etc. mixing at all! It's just so
//    you get something useful.

int stb_vorbis_get_samples_float_interleaved(stb_vorbis *f, int channels,
                                             float *buffer, int num_floats);
int stb_vorbis_get_samples_float(stb_vorbis *f, int channels, float **buffer,
                                 int num_samples);
// gets num_samples samples, not necessarily on a frame boundary--this requires
// buffering so you have to supply the buffers. DOES NOT APPLY THE COERCION
// RULES. Returns the number of samples stored per channel; it may be less than
// requested at the end of the file. If there are no more samples in the file,
// returns 0.

int stb_vorbis_get_samples_short_interleaved(stb_vorbis *f, int channels,
                                             short *buffer, int num_shorts);
int stb_vorbis_get_samples_short(stb_vorbis *f, int channels, short **buffer,
                                 int num_samples);
// gets num_samples samples, not necessarily on a frame boundary--this requires
// buffering so you have to supply the buffers. Applies the coercion rules above
// to produce 'channels' channels. Returns the number of samples stored per
// channel; it may be less than requested at the end of the file. If there are
// no more samples in the file, returns 0.

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_STB_STB_VORBIS_H_ */
