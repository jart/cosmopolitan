PL_MPEG - MPEG1 Video decoder, MP2 Audio decoder, MPEG-PS demuxer
Dominic Szablewski - https://phoboslab.org

-- Synopsis

// This function gets called for each decoded video frame
void my_video_callback(plm_t *plm, plm_frame_t *frame, void *user) {
	// Do something with frame->y.data, frame->cr.data, frame->cb.data
}

// This function gets called for each decoded audio frame
void my_audio_callback(plm_t *plm, plm_samples_t *frame, void *user) {
	// Do something with samples->interleaved
}

// Load a .mpg (MPEG Program Stream) file
plm_t *plm = plm_create_with_filename("some-file.mpg");

// Install the video & audio decode callbacks
plm_set_video_decode_callback(plm, my_video_callback, my_data);
plm_set_audio_decode_callback(plm, my_audio_callback, my_data);


// Decode
do {
	plm_decode(plm, time_since_last_call);
} while (!plm_has_ended(plm));

// All done
plm_destroy(plm);



-- Documentation

This library provides several interfaces to load, demux and decode MPEG video
and audio data. A high-level API combines the demuxer, video & audio decoders
in an easy to use wrapper.

Lower-level APIs for accessing the demuxer, video decoder and audio decoder,
as well as providing different data sources are also available.

Interfaces are written in an object orientet style, meaning you create object
instances via various different constructor functions (plm_*create()),
do some work on them and later dispose them via plm_*destroy().

plm_*		-- the high-level interface, combining demuxer and decoders
plm_buffer_* -- the data source used by all interfaces
plm_demux_*  -- the MPEG-PS demuxer
plm_video_*  -- the MPEG1 Video ("mpeg1") decoder
plm_audio_*  -- the MPEG1 Audio Layer II ("mp2") decoder


This library uses malloc(), realloc() and free() to manage memory. Typically
all allocation happens up-front when creating the interface. However, the
default buffer size may be too small for certain inputs. In these cases plmpeg
will realloc() the buffer with a larger size whenever needed. You can configure
the default buffer size by defining PLM_BUFFER_DEFAULT_SIZE *before*
including this library.

With the high-level interface you have two options to decode video & audio:

1) Use plm_decode() and just hand over the delta time since the last call.
It will decode everything needed and call your callbacks (specified through
plm_set_{video|audio}_decode_callback()) any number of times.

2) Use plm_decode_video() and plm_decode_audio() to decode exactly one
frame of video or audio data at a time. How you handle the synchronization of
both streams is up to you.

If you only want to decode video *or* audio through these functions, you should
disable the other stream (plm_set_{video|audio}_enabled(false))


Video data is decoded into a struct with all 3 planes (Y, Cr, Cb) stored in
separate buffers. You can either convert this to RGB on the CPU (slow) via the
plm_frame_to_rgb() function or do it on the GPU with the following matrix:

mat4 rec601 = mat4(
	1.16438,  0.00000,  1.59603, -0.87079,
	1.16438, -0.39176, -0.81297,  0.52959,
	1.16438,  2.01723,  0.00000, -1.08139,
	0, 0, 0, 1
);
gl_FragColor = vec4(y, cb, cr, 1.0) * rec601;

Audio data is decoded into a struct with either one single float array with the
samples for the left and right channel interleaved, or if the
PLM_AUDIO_SEPARATE_CHANNELS is defined *before* including this library, into
two separate float arrays - one for each channel.

See below for detailed the API documentation.
