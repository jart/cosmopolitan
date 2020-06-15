# AVIR #
## Introduction ##
Keywords: image resize, image resizer, image resizing, image scaling,
image scaler, image resize c++, image resizer c++

Please consider supporting the author on [Patreon](https://www.patreon.com/aleksey_vaneev).

Me, Aleksey Vaneev, is happy to offer you an open source image resizing /
scaling library which has reached a production level of quality, and is
ready to be incorporated into any project. This library features routines
for both down- and upsizing of 8- and 16-bit, 1 to 4-channel images. Image
resizing routines were implemented in multi-platform C++ code, and have a
high level of optimality. Beside resizing, this library offers a sub-pixel
shift operation. Built-in sRGB gamma correction is available.

The resizing algorithm at first produces 2X upsized image (relative to the
source image size, or relative to the destination image size if downsizing is
performed) and then performs interpolation using a bank of sinc function-based
fractional delay filters. At the last stage a correction filter is applied
which fixes smoothing introduced at previous steps.

The resizing algorithm was designed to provide the best visual quality. The
author even believes this algorithm provides the "ultimate" level of
quality (for an orthogonal resizing) which cannot be increased further: no
math exists to provide a better frequency response, better anti-aliasing
quality and at the same time having less ringing artifacts: these are 3
elements that define any resizing algorithm's quality; in AVIR practice these
elements have a high correlation to each other, so they can be represented by
a single parameter (AVIR offers several parameter sets with varying quality).
Algorithm's time performance turned out to be very good as well (for the
"ultimate" image quality).

An important element utilized by this algorithm is the so called Peaked Cosine
window function, which is applied over sinc function in all filters. Please
consult the documentation for more details.

Note that since AVIR implements orthogonal resizing, it may exhibit diagonal
aliasing artifacts. These artifacts are usually suppressed by EWA or radial
filtering techniques. EWA-like technique is not implemented in AVIR, because
it requires considerably more computing resources and may produce a blurred
image.

As a bonus, a faster `LANCIR` image resizing algorithm is also offered as a
part of this library. But the main focus of this documentation is the original
AVIR image resizing algorithm.

AVIR does not offer affine and non-linear image transformations "out of the
box". Since upsizing is a relatively fast operation in AVIR (required time
scales linearly with the output image area), affine and non-linear
transformations can be implemented in steps: 4- to 8-times upsizing,
transformation via bilinear interpolation, downsizing (linear proportional
affine transformations can probably skip the downsizing step). This should not
compromise the transformation quality much as bilinear interpolation's
problems will mostly reside in spectral area without useful signal, with a
maximum of 0.7 dB high-frequency attenuation for 4-times upsizing, and 0.17 dB
attenuation for 8-times upsizing. This approach is probably as time efficient
as performing a high-quality transform over the input image directly (the only
serious drawback is the increased memory requirement). Note that affine
transformations that change image proportions should first apply proportion
change during upsizing.

*AVIR is devoted to women. Your digital photos can look good at any size!*

## Requirements ##
C++ compiler and system with efficient "float" floating point (24-bit
mantissa) type support. This library can also internally use the "double" and
SIMD floating point types during resizing if needed. This library does not
have dependencies beside the standard C library.

## Links ##
* [Documentation](https://www.voxengo.com/public/avir/Documentation/)

## Usage Information ##
The image resizer is represented by the `avir::CImageResizer<>` class, which
is a single front-end class for the whole library. Basically, you do not need
to use nor understand any other classes beside this class.

The code of the library resides in the "avir" C++ namespace, effectively
isolating it from all other code. The code is thread-safe. You need just
a single resizer object per running application, at any time, even when
resizing images concurrently.

To resize images in your application, simply add 3 lines of code:

    #include "avir.h"
    avir :: CImageResizer<> ImageResizer( 8 );
    ImageResizer.resizeImage( InBuf, 640, 480, 0, OutBuf, 1024, 768, 3, 0 );
    (multi-threaded operation requires additional coding, see the documentation)

For low-ringing performance:

    avir :: CImageResizer<> ImageResizer( 8, 0, avir :: CImageResizerParamsLR() );

To use the built-in gamma correction, an object of the
`avir::CImageResizerVars` class with its variable `UseSRGBGamma` set to "true"
should be supplied to the `resizeImage()` function. Note that the gamma
correction is applied to all channels (e.g. alpha-channel) in the current
implementation.

    avir :: CImageResizerVars Vars;
    Vars.UseSRGBGamma = true;

Dithering (error-diffusion dither which is perceptually good) can be enabled
this way:

    typedef avir :: fpclass_def< float, float,
        avir :: CImageResizerDithererErrdINL< float > > fpclass_dith;
    avir :: CImageResizer< fpclass_dith > ImageResizer( 8 );

The library is able to process images of any bit depth: this includes 8-bit,
16-bit, float and double types. Larger integer and signed integer types are
not supported. Supported source and destination image sizes are only limited
by the available system memory.

The code of this library was commented in the [Doxygen](http://www.doxygen.org/)
style. To generate the documentation locally you may run the
`doxygen ./other/avirdoxy.txt` command from the library's directory. Note that
the code was suitably documented allowing you to make modifications, and to
gain full understanding of the algorithm.

Preliminary tests show that this library (compiled with Intel C++ Compiler
18.2 with AVX2 instructions enabled, without explicit SIMD resizing code) can
resize 8-bit RGB 5184x3456 (17.9 Mpixel) 3-channel image down to 1920x1280
(2.5 Mpixel) image in 245 milliseconds, utilizing a single thread, on Intel
Core i7-7700K processor-based system without overclocking. This scales down to
74 milliseconds if 8 threads are utilized.

Multi-threaded operation is not provided by this library "out of the box".
The multi-threaded (horizontally-threaded) infrastructure is available, but
requires additional system-specific interfacing code for engagement.

## SIMD Usage Information ##
This library is capable of using SIMD floating point types for internal
variables. This means that up to 4 color channels can be processed in
parallel. Since the default interleaved processing algorithm itself remains
non-SIMD, the use of SIMD internal types is not practical for 1- and 2-channel
image resizing (due to overhead). SIMD internal type can be used this way:

    #include "avir_float4_sse.h"
    avir :: CImageResizer< avir :: fpclass_float4 > ImageResizer( 8 );

For 1-channel and 2-channel image resizing when AVX instructions are allowed
it may be reasonable to utilize de-interleaved SIMD processing algorithm.
While it gives no performance benefit if the "float4" SSE processing type is
used, it offers some performance boost if the "float8" AVX processing type is
used (given dithering is not performed, or otherwise performance is reduced at
the dithering stage since recursive dithering cannot be parallelized). The
internal type remains non-SIMD "float". De-interleaved algorithm can be used
this way:

    #include "avir_float8_avx.h"
    avir :: CImageResizer< avir :: fpclass_float8_dil > ImageResizer( 8 );

It's important to note that on the latest Intel processors (i7-7700K and
probably later) the use of the aforementioned SIMD-specific resizing code may
not be justifiable, or may be even counter-productive due to many factors:
memory bandwidth bottleneck, increased efficiency of processor's circuitry
utilization and out-of-order execution, automatic SIMD optimizations performed
by the compiler. This is at least true when compiling 64-bit code with Intel
C++ Compiler 18.2 with /QxSSE4.2, or especially with the /QxCORE-AVX2 option.
SSE-specific resizing code may still be a little bit more efficient for
4-channel image resizing.

## Notes ##
This library was tested for compatibility with [GNU C++](http://gcc.gnu.org/),
[Microsoft Visual C++](http://www.microsoft.com/visualstudio/eng/products/visual-studio-express-products)
and [Intel C++](http://software.intel.com/en-us/c-compilers) compilers, on 32-
and 64-bit Windows, macOS and CentOS Linux. The code was also tested with
Dr.Memory/Win32 for the absence of uninitialized or unaddressable memory
accesses.

All code is fully "inline", without the need to compile any source files. The
memory footprint of the library itself is very modest, except that the size of
the temporary image buffers depends on the input and output image sizes, and
is proportionally large.

The "heart" of resizing algorithm's quality resides in the parameters defined
via the `avir::CImageResizerParams` structure. While the default set of
parameters that offers a good quality was already provided, there is
(probably) still a place for improvement exists, and the default parameters
may change in a future update. If you need to recall an exact set of
parameters, simply save them locally for a later use.

When the algorithm is run with no resizing applied (k=1), the result of
resizing will not be an exact, but a very close copy of the source image. The
reason for such inexactness is that the image is always low-pass filtered at
first to reduce aliasing during subsequent resizing, and at last filtered by a
correction filter. Such approach allows algorithm to maintain a stable level
of quality regardless of the resizing "k" factor used.

This library includes a binary command line tool "imageresize" for major
desktop platforms. This tool was designed to be used as a demonstration of
library's performance, and as a reference, it is multi-threaded (the `-t`
switch can be used to control the number of threads utilized). This tool uses
plain "float" processing (no explicit SIMD) and relies on automatic compiler
optimization (with Win64 binary being the "main" binary as it was compiled
with the best ICC optimization options for the time being). This tool uses the
following libraries:
* turbojpeg Copyright (c) 2009-2013 D. R. Commander
* libpng Copyright (c) 1998-2013 Glenn Randers-Pehrson
* zlib Copyright (c) 1995-2013 Jean-loup Gailly and Mark Adler

Note that you can enable gamma-correction with the `-g` switch. However,
sometimes gamma-correction produces "greenish/reddish/bluish haze" since
low-amplitude oscillations produced by resizing at object boundaries are
amplified by gamma correction. This can also have an effect of reduced
contrast.

## Interpolation Discussion ##
The use of certain low-pass filters and 2X upsampling in this library is
hardly debatable, because they are needed to attain a certain anti-aliasing
effect and keep ringing artifacts low. But the use of sinc function-based
interpolation filter that is 18 taps-long (may be higher, up to 36 taps in
practice) can be questioned, because even in 0th order case such
interpolation filter requires 18 multiply-add operations. Comparatively, an
optimal Hermite or cubic interpolation spline requires 8 multiply and 11 add
operations.

One of the reasons 18-tap filter is preferred, is because due to memory
bandwidth limitations using a lower-order filter does not provide any
significant performance increase (e.g. 14-tap filter is less than 5% more
efficient overall). At the same time, in comparison to cubic spline, 18-tap
filter embeds a low-pass filter that rejects signal above 0.5\*pi (provides
additional anti-aliasing filtering), and this filter has a consistent shape at
all fractional offsets. Splines have a varying low-pass filter shape at
different fractional offsets (e.g. no low-pass filtering at 0.0 offset,
and maximal low-pass filtering at 0.5 offset). 18-tap filter also offers a
superior stop-band attenuation which almost guarantees absence of artifacts if
the image is considerably sharpened afterwards.

## Why 2X upsizing in AVIR? ##
Classic approaches to image resizing do not perform an additional 2X upsizing.
So, why such upsizing is needed at all in AVIR? Indeed, image resizing can be
implemented using a single interpolation filter which is applied to the source
image directly. However, such approach has limitations:

First of all, speaking about non-2X-upsized resizing, during upsizing the
interpolation filter has to be tuned to a frequency close to pi (Nyquist) in
order to reduce high-frequency smoothing: this reduces the space left for
filter optimization. Beside that, during downsizing, a filter that performs
well and predictable when tuned to frequencies close to the Nyquist frequency,
may become distorted in its spectral shape when it is tuned to lower
frequencies. That is why it is usually a good idea to have filter's stop-band
begin below Nyquist so that the transition band's shape remains stable at any
lower-frequency setting. At the same time, this requirement complicates a
further corrective filtering, because correction filter may become too steep
at the point where the stop-band begins.

Secondly, speaking about non-2X-upsized resizing, filter has to be very short
(with a base length of 5-7 taps, further multiplied by the resizing factor) or
otherwise the ringing artifacts will be very strong: it is a general rule that
the steeper the filter is around signal frequencies being removed the higher
the ringing artifacts are. That is why it is preferred to move steep
transitions into the spectral area with a quieter signal. A short filter also
means it cannot provide a strong "beyond-Nyquist" stop-band attenuation, so an
interpolated image will look a bit edgy or not very clean due to stop-band
artifacts.

To sum up, only additional controlled 2X upsizing provides enough spectral
space to design interpolation filter without visible ringing artifacts yet
providing a strong stop-band attenuation and stable spectral characteristics
(good at any resizing "k" factor). Moreover, 2X upsizing becomes very
important in maintaining a good resizing quality when downsizing and upsizing
by small "k" factors, in the range 0.5 to 2: resizing approaches that do not
perform 2X upsizing usually cannot design a good interpolation filter for such
factors just because there is not enough spectral space available.

## Why Peaked Cosine in AVIR? ##
First of all, AVIR is a general solution to image resizing problem. That is
why it should not be directly compared to "spline interpolation" or "Lanczos
resampling", because the latter two are only means to design interpolation
filters, and they can be implemented in a variety of ways, even in sub-optimal
ways. Secondly, with only a minimal effort AVIR can be changed to use any
existing interpolation formula and any window function, but this is just not
needed.

An effort was made to compare Peaked Cosine to Lanczos window function, and
here is the author's opinion. Peaked Cosine has two degrees of freedom whereas
Lanczos has one degree of freedom. While both functions can be used with
acceptable results, Peaked Cosine window function used in automatic parameter
optimization really pushes the limits of frequency response linearity,
anti-aliasing strength (stop-band attenuation) and low-ringing performance
which Lanczos cannot usually achieve. This is true at least when using a
general-purpose downhill simplex optimization method. Lanczos window has good
(but not better) characteristics in several special cases (certain "k"
factors) which makes it of limited use in a general solution such as AVIR.

Among other window functions (Kaiser, Gaussian, Cauchy, Poisson, generalized
cosine windows) there are no better candidates as well. It looks like Peaked
Cosine function's scalability (it retains stable, almost continously-variable
spectral characteristics at any window parameter values), and its ability to
create "desirable" pass-band ripple in the frequency response near the cutoff
point contribute to its better overall quality. Somehow Peaked Cosine window
function optimization manages to converge to reasonable states in most cases
(that is why AVIR library comes with a set of equally robust, but distinctive
parameter sets) whereas all other window functions tend to produce
unpredictable optimization results.

The only disadvantage of Peaked Cosine window function is that usable filters
windowed by this function tend to be longer than "usual" (with Kaiser window
being the "golden standard" for filter length per decibel of stop-band
attenuation). This is a price that should be paid for stable spectral
characteristics.

## LANCIR ##

As a part of AVIR library, the `CLancIR` class is also offered which is an
optimal implementation of *Lanczos* image resizing filter. This class has a
similar programmatic interface to AVIR, but it is not thread-safe: each
executing thread should have its own `CLancIR` object. This class was designed
for cases of batch processing of same-sized frames like in video encoding.

LANCIR offers up to 200% faster image resizing in comparison to AVIR. The
quality difference is, however, debatable. Note that while LANCIR can take
8- and 16-bit and float image buffers, its precision is limited to 8-bit
resizing.

LANCIR should be seen as a bonus and as some kind of quality comparison.
LANCIR uses Lanczos filter "a" parameter equal to 3 which is similar to AVIR's
default setting.

## Change log ##
Version 2.4:

* Removed outdated `_mm_reset()` function calls from the SIMD code.
* Changed `float4 round()` to use SSE2 rounding features, avoiding use of
64-bit registers.

Version 2.3:

* Implemented CLancIR image resizing algorithm.
* Fixed a minor image offset on image upsizing.

Version 2.2:

* Released AVIR under a permissive MIT license agreement.

Version 2.1:

* Fixed error-diffusion dither problems introduced in the previous version.
* Added the `-1` switch to the `imageresize` to enable 1-bit output for
dither's quality evaluation (use together with the `-d` switch).
* Added the `--algparams=` switch to the `imageresize` to control resizing
quality (replaces the `--low-ring` switch).
* Added `avir :: CImageResizerParamsULR` parameter set for lowest-ringing
performance possible (not considerably different to
`avir :: CImageResizerParamsLR`, but a bit lower ringing).

Version 2.0:

* Minor inner loop optimizations.
* Lifted the supported image size constraint by switching buffer addressing to
`size_t` from `int`, now image size is limited by the available system memory.
* Added several useful switches to the `imageresize` utility.
* Now `imageresize` does not apply gamma-correction by default.
* Fixed scaling of bit depth-reduction operation.
* Improved error-diffusion dither's signal-to-noise ratio.
* Compiled binaries with AVX2 instruction set (SSE4 for macOS).

## Users ##
This library is used by:

  * [Contaware.com](http://www.contaware.com/)

Please drop me a note at aleksey.vaneev@gmail.com and I will include a link to
your software product to the list of users. This list is important at
maintaining confidence in this library among the interested parties.
