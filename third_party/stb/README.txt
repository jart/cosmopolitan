/* stb_image - v2.29 - public domain image loader - http://nothings.org/stb
 *                                no warranty implied; use at your own risk
 *
 * [heavily modified by justine tunney]
 *
 *    JPEG baseline & progressive (12 bpc/arithmetic not supported, same
 *                      as stock IJG lib)
 *    PNG 1/2/4/8/16-bit-per-channel
 *    GIF (*comp always reports as 4-channel)
 *    PNM (PPM and PGM binary only)
 *
 *    Animated GIF still needs a proper API, but here's one way to do it:
 *        http://gist.github.com/urraka/685d9a6340b26b830d49
 *
 *    - decode from memory or through FILE (define STBI_NO_STDIO to remove code)
 *    - decode from arbitrary I/O callbacks
 *
 * ============================    Contributors    =========================
 *
 * Image formats                         Extensions, features
 *  Sean Barrett (jpeg, png, bmp)         Jetro Lauha (stbi_info)
 *  Nicolas Schulz (hdr, psd)             Martin "SpartanJ" Golini (stbi_info)
 *  Jonathan Dummer (tga)                 James "moose2000" Brown (iPhone PNG)
 *  Jean-Marc Lienher (gif)               Ben "Disch" Wenger (io callbacks)
 *  Tom Seddon (pic)                      Omar Cornut (1/2/4-bit PNG)
 *  Thatcher Ulrich (psd)                 Nicolas Guillemot (vertical flip)
 *  Ken Miller (pgm, ppm)                 Richard Mitton (16-bit PSD)
 *  github:urraka (animated gif)          Junggon Kim (PNM comments)
 *  Christopher Forseth (animated gif)    Daniel Gibson (16-bit TGA)
 *                                        socks-the-fox (16-bit PNG)
 * Optimizations & bugfixes               Jeremy Sawicki (ImageNet JPGs)
 *  Fabian "ryg" Giesen                   Mikhail Morozov (1-bit BMP)
 *  Arseny Kapoulkine                     Anael Seghezzi (is-16-bit query)
 *  John-Mark Allen                       Simon Breuss (16-bit PNM)
 *  Carmelo J Fdez-Aguera
 *
 * Bug & warning fixes
 *  Marc LeBlanc            Laurent Gomila         JR Smith
 *  Christpher Lloyd        Sergio Gonzalez        Matvey Cherevko
 *  Phil Jordan             Ryamond Barbiero       Zack Middleton
 *  Hayaki Saito            Engin Manap
 *  Luke Graham             Dale Weiler            Martins Mozeiko
 *  Thomas Ruf              Neil Bickford          Blazej Dariusz Roszkowski
 *  Janez Zemva             Gregory Mullen         Roy Eltham
 *  Jonathan Blow           Kevin Schmidt
 *  Eugene Golushkov        Brad Weinberger        the Horde3D community
 *  Aruelien Pocheville     Alexander Veselov      github:rlyeh
 *  Cass Everitt            [reserved]             github:romigrou
 *  Paul Du Bois                                   github:svdijk
 *  Philipp Wiesemann       Guillaume George       github:snagar
 *  Josh Tobin              Joseph Thomson         github:Zelex
 *  Julian Raschke          Dave Moore             github:grim210
 *  Baldur Karlsson         Won Chun               github:sammyhw
 *                          Nick Verigakis         github:phprus
 *  Luca Sas                                       github:poppolopoppo
 *  Ryan C. Gordon          Michal Cichon          github:darealshinji
 *  David Woo               Tero Hanninen          github:Michaelangel007
 *  Jerry Jansson           Cort Stratton          github:mosra
 *                          Thibault Reuille       [reserved]
 *  Nathan Reed                                    [reserved]
 *  Johan Duparc            Aldo Culquicondor
 *  Ronny Chevalier         Oriol Ferrer           Jacko Dirks
 *  John Bartholomew        Matthew Gregan
 *  Ken Hamada              Christian Floisand
 *
 * ============================    Documentation   =========================
 *
 * Limitations:
 *    - no 12-bit-per-channel JPEG
 *    - no JPEGs with arithmetic coding
 *    - GIF always returns *comp=4
 *
 * Basic usage (see HDR discussion below for HDR usage):
 *    int x,y,n;
 *    unsigned char *data = stbi_load(filename, &x, &y, &n, 0);
 *    // ... process data if not NULL ...
 *    // ... x = width, y = height, n = # 8-bit components per pixel ...
 *    // ... replace '0' with '1'..'4' to force that many components per pixel
 *    // ... but 'n' will always be the number that it would have been if you
 *    // ... said 0
 *    stbi_image_free(data);
 *
 * Standard parameters:
 *    int *x                 -- outputs image width in pixels
 *    int *y                 -- outputs image height in pixels
 *    int *channels_in_file  -- outputs # of image components in image file
 *    int desired_channels   -- if non-zero, # of image components requested in
 *                              result
 *
 * The return value from an image loader is an 'unsigned char *' which points
 * to the pixel data, or NULL on an allocation failure or if the image is
 * corrupt or invalid. The pixel data consists of *y scanlines of *x pixels,
 * with each pixel consisting of N interleaved 8-bit components; the first
 * pixel pointed to is top-left-most in the image. There is no padding between
 * image scanlines or between pixels, regardless of format. The number of
 * components N is 'desired_channels' if desired_channels is non-zero, or
 * *channels_in_file otherwise. If desired_channels is non-zero,
 * *channels_in_file has the number of components that _would_ have been
 * output otherwise. E.g. if you set desired_channels to 4, you will always
 * get RGBA output, but you can check *channels_in_file to see if it's trivially
 * opaque because e.g. there were only 3 channels in the source image.
 *
 * An output image with N components has the following components interleaved
 * in this order in each pixel:
 *
 *     N=#comp     components
 *       1           grey
 *       2           grey, alpha
 *       3           red, green, blue
 *       4           red, green, blue, alpha
 *
 * If image loading fails for any reason, the return value will be NULL,
 * and *x, *y, *channels_in_file will be unchanged. The function
 * stbi_failure_reason() can be queried for an extremely brief, end-user
 * unfriendly explanation of why the load failed. Define STBI_NO_FAILURE_STRINGS
 * to avoid compiling these strings at all, and STBI_FAILURE_USERMSG to get
 * slightly more user-friendly ones.
 *
 * Paletted PNG, BMP, GIF, and PIC images are automatically depalettized.
 *
 * To query the width, height and component count of an image without having to
 * decode the full file, you can use the stbi_info family of functions:
 *
 *   int x,y,n,ok;
 *   ok = stbi_info(filename, &x, &y, &n);
 *   // returns ok=1 and sets x, y, n if image is a supported format,
 *   // 0 otherwise.
 *
 * Note that stb_image pervasively uses ints in its public API for sizes,
 * including sizes of memory buffers. This is now part of the API and thus
 * hard to change without causing breakage. As a result, the various image
 * loaders all have certain limits on image size; these differ somewhat
 * by format but generally boil down to either just under 2GB or just under
 * 1GB. When the decoded image would be larger than this, stb_image decoding
 * will fail.
 *
 * Additionally, stb_image will reject image files that have any of their
 * dimensions set to a larger value than the configurable STBI_MAX_DIMENSIONS,
 * which defaults to 2**24 = 16777216 pixels. Due to the above memory limit,
 * the only way to have an image with such dimensions load correctly
 * is for it to have a rather extreme aspect ratio. Either way, the
 * assumption here is that such larger images are likely to be malformed
 * or malicious. If you do need to load an image with individual dimensions
 * larger than that, and it still fits in the overall size limit, you can
 * #define STBI_MAX_DIMENSIONS on your own to be something larger.
 *
 * ===========================================================================
 *
 * I/O callbacks
 *
 * I/O callbacks allow you to read from arbitrary sources, like packaged
 * files or some other source. Data read from callbacks are processed
 * through a small internal buffer (currently 128 bytes) to try to reduce
 * overhead.
 *
 * The three functions you must define are "read" (reads some bytes of data),
 * "skip" (skips some bytes of data), "eof" (reports if the stream is at the
 * end).
 *
 * ===========================================================================
 *
 * HDR image support   (disable by defining STBI_NO_HDR)
 *
 * stb_image supports loading HDR images in general, and currently the Radiance
 * .HDR file format specifically. You can still load any file through the
 * existing interface; if you attempt to load an HDR file, it will be
 * automatically remapped to LDR, assuming gamma 2.2 and an arbitrary scale
 * factor defaulting to 1; both of these constants can be reconfigured through
 * this interface:
 *
 *     stbi_hdr_to_ldr_gamma(2.2f);
 *     stbi_hdr_to_ldr_scale(1.0f);
 *
 * (note, do not use _inverse_ constants; stbi_image will invert them
 * appropriately).
 *
 * Additionally, there is a new, parallel interface for loading files as
 * (linear) floats to preserve the full dynamic range:
 *
 *    float *data = stbi_loadf(filename, &x, &y, &n, 0);
 *
 * If you load LDR images through this interface, those images will
 * be promoted to floating point values, run through the inverse of
 * constants corresponding to the above:
 *
 *     stbi_ldr_to_hdr_scale(1.0f);
 *     stbi_ldr_to_hdr_gamma(2.2f);
 *
 * Finally, given a filename (or an open file or memory block--see header
 * file for details) containing image data, you can query for the "most
 * appropriate" interface to use (that is, whether the image is HDR or
 * not), using:
 *
 *     stbi_is_hdr(char *filename);
 *
 * ===========================================================================
 *
 * iPhone PNG support:
 *
 * We optionally support converting iPhone-formatted PNGs (which store
 * premultiplied BGRA) back to RGB, even though they're internally encoded
 * differently. To enable this conversion, call
 * stbi_convert_iphone_png_to_rgb(1).
 *
 * Call stbi_set_unpremultiply_on_load(1) as well to force a divide per
 * pixel to remove any premultiplied alpha *only* if the image file explicitly
 * says there's premultiplied data (currently only happens in iPhone images,
 * and only if iPhone convert-to-rgb processing is on).
 *
 * ===========================================================================
 *
 * ADDITIONAL CONFIGURATION
 *
 *  - You can suppress implementation of any of the decoders to reduce
 *    your code footprint by #defining one or more of the following
 *    symbols before creating the implementation.
 *
 *        STBI_NO_JPEG
 *        STBI_NO_PNG
 *        STBI_NO_GIF
 *        STBI_NO_HDR
 *        STBI_NO_PNM   (.ppm and .pgm)
 *
 *   - If you use STBI_NO_PNG (or _ONLY_ without PNG), and you still
 *     want the zlib decoder to be available, #define STBI_SUPPORT_ZLIB
 *
 *  - If you define STBI_MAX_DIMENSIONS, stb_image will reject images greater
 *    than that size (in either width or height) without further processing.
 *    This is to let programs in the wild set an upper bound to prevent
 *    denial-of-service attacks on untrusted data, as one could generate a
 *    valid image of gigantic dimensions and force stb_image to allocate a
 *    huge block of memory and spend disproportionate time decoding it. By
 *    default this is set to (1 << 24), which is 16777216, but that's still
 *    very big.
 *
 */

/* stb_image_resize - v0.97 - public domain image resizing
 * by Jorge L Rodriguez (@VinoBS) - 2014
 * http://github.com/nothings/stb
 *
 * Written with emphasis on usability, portability, and efficiency. (No
 * SIMD or threads, so it be easily outperformed by libs that use those.)
 * Only scaling and translation is supported, no rotations or shears.
 * Easy API downsamples w/Mitchell filter, upsamples w/cubic interpolation.
 *
 * QUICKSTART
 *    stbir_resize_uint8(      input_pixels , in_w , in_h , 0,
 *                             output_pixels, out_w, out_h, 0, num_channels)
 *    stbir_resize_float(...)
 *    stbir_resize_uint8_srgb( input_pixels , in_w , in_h , 0,
 *                             output_pixels, out_w, out_h, 0,
 *                             num_channels , alpha_chan  , 0)
 *    stbir_resize_uint8_srgb_edgemode(
 *                             input_pixels , in_w , in_h , 0,
 *                             output_pixels, out_w, out_h, 0,
 *                             num_channels , alpha_chan  , 0, STBIR_EDGE_CLAMP)
 *                                                          // WRAP/REFLECT/ZERO
 *
 * DOCUMENTATION
 *
 *    SRGB & FLOATING POINT REPRESENTATION
 *       The sRGB functions presume IEEE floating point. If you do not have
 *       IEEE floating point, define STBIR_NON_IEEE_FLOAT. This will use
 *       a slower implementation.
 *
 *    MEMORY ALLOCATION
 *       The resize functions here perform a single memory allocation using
 *       malloc. To control the memory allocation, before the #include that
 *       triggers the implementation, do:
 *
 *          #define STBIR_MALLOC(size,context) ...
 *          #define STBIR_FREE(ptr,context)   ...
 *
 *       Each resize function makes exactly one call to malloc/free, so to use
 *       temp memory, store the temp memory in the context and return that.
 *
 *    ASSERT
 *       Define STBIR_ASSERT(boolval) to override assert() and not use assert.h
 *
 *    OPTIMIZATION
 *       Define STBIR_SATURATE_INT to compute clamp values in-range using
 *       integer operations instead of float operations. This may be faster
 *       on some platforms.
 *
 *    DEFAULT FILTERS
 *       For functions which don't provide explicit control over what filters
 *       to use, you can change the compile-time defaults with
 *
 *          #define STBIR_DEFAULT_FILTER_UPSAMPLE     STBIR_FILTER_something
 *          #define STBIR_DEFAULT_FILTER_DOWNSAMPLE   STBIR_FILTER_something
 *
 *       See stbir_filter in the header-file section for the list of filters.
 *
 *    NEW FILTERS
 *       A number of 1D filter kernels are used. For a list of
 *       supported filters see the stbir_filter enum. To add a new filter,
 *       write a filter function and add it to stbir__filter_info_table.
 *
 *    PROGRESS
 *       For interactive use with slow resize operations, you can install
 *       a progress-report callback:
 *
 *          #define STBIR_PROGRESS_REPORT(val)   some_func(val)
 *
 *       The parameter val is a float which goes from 0 to 1 as progress
 *       is made.
 *
 *       For example:
 *
 *          static void my_progress_report(float progress);
 *          #define STBIR_PROGRESS_REPORT(val) my_progress_report(val)
 *
 *          #define STB_IMAGE_RESIZE_IMPLEMENTATION
 *
 *          static void my_progress_report(float progress)
 *          {
 *             printf("Progress: %f%%\n", progress*100);
 *          }
 *
 *    MAX CHANNELS
 *       If your image has more than 64 channels, define STBIR_MAX_CHANNELS
 *       to the max you'll have.
 *
 *    ALPHA CHANNEL
 *       Most of the resizing functions provide the ability to control how
 *       the alpha channel of an image is processed. The important things
 *       to know about this:
 *
 *       1. The best mathematically-behaved version of alpha to use is
 *       called "premultiplied alpha", in which the other color channels
 *       have had the alpha value multiplied in. If you use premultiplied
 *       alpha, linear filtering (such as image resampling done by this
 *       library, or performed in texture units on GPUs) does the "right
 *       thing". While premultiplied alpha is standard in the movie CGI
 *       industry, it is still uncommon in the videogame/real-time world.
 *
 *       If you linearly filter non-premultiplied alpha, strange effects
 *       occur. (For example, the 50/50 average of 99% transparent bright green
 *       and 1% transparent black produces 50% transparent dark green when
 *       non-premultiplied, whereas premultiplied it produces 50%
 *       transparent near-black. The former introduces green energy
 *       that doesn't exist in the source image.)
 *
 *       2. Artists should not edit premultiplied-alpha images; artists
 *       want non-premultiplied alpha images. Thus, art tools generally output
 *       non-premultiplied alpha images.
 *
 *       3. You will get best results in most cases by converting images
 *       to premultiplied alpha before processing them mathematically.
 *
 *       4. If you pass the flag STBIR_FLAG_ALPHA_PREMULTIPLIED, the
 *       resizer does not do anything special for the alpha channel;
 *       it is resampled identically to other channels. This produces
 *       the correct results for premultiplied-alpha images, but produces
 *       less-than-ideal results for non-premultiplied-alpha images.
 *
 *       5. If you do not pass the flag STBIR_FLAG_ALPHA_PREMULTIPLIED,
 *       then the resizer weights the contribution of input pixels
 *       based on their alpha values, or, equivalently, it multiplies
 *       the alpha value into the color channels, resamples, then divides
 *       by the resultant alpha value. Input pixels which have alpha=0 do
 *       not contribute at all to output pixels unless _all_ of the input
 *       pixels affecting that output pixel have alpha=0, in which case
 *       the result for that pixel is the same as it would be without
 *       STBIR_FLAG_ALPHA_PREMULTIPLIED. However, this is only true for
 *       input images in integer formats. For input images in float format,
 *       input pixels with alpha=0 have no effect, and output pixels
 *       which have alpha=0 will be 0 in all channels. (For float images,
 *       you can manually achieve the same result by adding a tiny epsilon
 *       value to the alpha channel of every image, and then subtracting
 *       or clamping it at the end.)
 *
 *       6. You can suppress the behavior described in #5 and make
 *       all-0-alpha pixels have 0 in all channels by #defining
 *       STBIR_NO_ALPHA_EPSILON.
 *
 *       7. You can separately control whether the alpha channel is
 *       interpreted as linear or affected by the colorspace. By default
 *       it is linear; you almost never want to apply the colorspace.
 *       (For example, graphics hardware does not apply sRGB conversion
 *       to the alpha channel.)
 *
 * CONTRIBUTORS
 *    Jorge L Rodriguez: Implementation
 *    Sean Barrett: API design, optimizations
 *    Aras Pranckevicius: bugfix
 *    Nathan Reed: warning fixes
 *
 * REVISIONS
 *    0.97 (2020-02-02) fixed warning
 *    0.96 (2019-03-04) fixed warnings
 *    0.95 (2017-07-23) fixed warnings
 *    0.94 (2017-03-18) fixed warnings
 *    0.93 (2017-03-03) fixed bug with certain combinations of heights
 *    0.92 (2017-01-02) fix integer overflow on large (>2GB) images
 *    0.91 (2016-04-02) fix warnings; fix handling of subpixel regions
 *    0.90 (2014-09-17) first released version
 *
 * LICENSE
 *   See end of file for license information.
 *
 * TODO
 *    Don't decode all of the image data when only processing a partial tile
 *    Don't use full-width decode buffers when only processing a partial tile
 *    When processing wide images, break processing into tiles so data fits in
 *    L1 cache Installable filters? Resize that respects alpha test coverage
 *       (Reference code: FloatImage::alphaTestCoverage and
 * FloatImage::scaleAlphaToCoverage:
 *       https://code.google.com/p/nvidia-texture-tools/source/browse/trunk/src/nvimage/FloatImage.cpp
 * )
 */
