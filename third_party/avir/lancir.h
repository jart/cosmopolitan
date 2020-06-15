// clang-format off
//$ nobt
//$ nocpp

/**
 * @file lancir.h
 *
 * @brief The self-contained "lancir" inclusion file.
 *
 * This is the self-contained inclusion file for the "LANCIR" image resizer,
 * part of the AVIR library.
 *
 * AVIR Copyright (c) 2015-2019 Aleksey Vaneev
 *
 * @mainpage
 *
 * @section intro_sec Introduction
 *
 * Description is available at https://github.com/avaneev/avir
 *
 * @section license License
 *
 * AVIR License Agreement
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015-2019 Aleksey Vaneev
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef AVIR_CLANCIR_INCLUDED
#define AVIR_CLANCIR_INCLUDED

#include "third_party/avir/notice.h"
#include "libc/str/str.h"
#include "libc/log/log.h"
#include "libc/mem/mem.h"
#include "libc/log/check.h"
#include "libc/macros.h"
#include "libc/math.h"

namespace avir {

/**
 * The macro equals to "pi" constant, fills 53-bit floating point mantissa.
 * Undefined at the end of file.
 */

#define LANCIR_PI 3.1415926535897932

/**
 * @brief LANCIR image resizer class.
 *
 * The object of this class can be used to resize 1-4 channel images to any
 * required size. Resizing is performed by utilizing Lanczos filters, with
 * 8-bit precision. This class offers a kind of "optimal" Lanczos resampling
 * implementation.
 *
 * Object of this class can be allocated on stack.
 *
 * Note that object of this class does not free temporary buffers and
 * variables after the resizeImage() call (until object's destruction), these
 * buffers are reused on subsequent calls making batch resizing of same-size
 * images faster. This means resizing is not thread-safe: a separate object
 * should be created for each thread.
 */

class CLancIR
{
private:
	CLancIR( const CLancIR& )
	{
		// Unsupported.
	}

	CLancIR& operator = ( const CLancIR& )
	{
		// Unsupported.
		return( *this );
	}

public:
	CLancIR()
		: FltBuf( NULL )
		, FltBufLen( 0 )
		, spv( NULL )
		, spvlen( 0 )
	{
	}

	~CLancIR()
	{
		delete[] FltBuf;
		delete[] spv;
	}

	/**
	 * Function resizes image.
	 *
	 * @param SrcBuf Source image buffer.
	 * @param SrcWidth Source image width.
	 * @param SrcHeight Source image height.
	 * @param SrcScanlineSize Physical size of source scanline in elements
	 * (not bytes). If this value is below 1, SrcWidth * ElCount will be
	 * used as the physical source scanline size.
	 * @param[out] NewBuf Buffer to accept the resized image. Can be equal to
	 * SrcBuf if the size of the resized image is smaller or equal to source
	 * image in size.
	 * @param NewWidth New image width.
	 * @param NewHeight New image height.
	 * @param ElCount The number of elements (channels) used to store each
	 * source and destination pixel (1-4).
	 * @param kx0 Resizing step - horizontal (one output pixel corresponds to
	 * "k" input pixels). A downsizing factor if > 1.0; upsizing factor
	 * if <= 1.0. Multiply by -1 if you would like to bypass "ox" and "oy"
	 * adjustment which is done by default to produce a centered image. If
	 * step value equals 0, the step value will be chosen automatically.
	 * @param ky0 Resizing step - vertical. Same as "kx".
	 * @param ox Start X pixel offset within source image (can be negative).
	 * Positive offset moves the image to the left.
	 * @param oy Start Y pixel offset within source image (can be negative).
	 * Positive offset moves the image to the top.
	 * @tparam T Input and output buffer element's type. Can be uint8_t
	 * (0-255 value range), uint16_t (0-65535 value range), float
	 * (any value range), double (any value range). Larger integer types are
	 * treated as uint16_t. Signed integer types are unsupported.
	 */

	template< class T >
	void resizeImage( const T* const SrcBuf, const int SrcWidth,
		const int SrcHeight, int SrcScanlineSize, T* const NewBuf,
		const int NewWidth, const int NewHeight, const int ElCount,
		const double kx0 = 0.0, const double ky0 = 0.0, double ox = 0.0,
		double oy = 0.0 )
	{
		if( NewWidth <= 0 || NewHeight <= 0 )
		{
			return;
		}

		if( SrcWidth <= 0 || SrcHeight <= 0 )
		{
			handleEmptySrcCornerCase( NewBuf, (size_t) NewWidth * NewHeight * sizeof( T ) );
			return;
		}

		const double la = 3.0; // Lanczos "a".
		double kx;
		double ky;

		if( kx0 == 0.0 )
		{
			if( NewWidth > SrcWidth )
			{
				kx = (double) ( SrcWidth - 1 ) / ( NewWidth - 1 );
			}
			else
			{
				kx = (double) SrcWidth / NewWidth;
				ox += ( kx - 1.0 ) * 0.5;
			}
		}
		else
		if( kx0 > 0.0 )
		{
			kx = kx0;

			if( kx0 > 1.0 )
			{
				ox += ( kx0 - 1.0 ) * 0.5;
			}
		}
		else
		{
			kx = -kx0;
		}

		if( ky0 == 0.0 )
		{
			if( NewHeight > SrcHeight )
			{
				ky = (double) ( SrcHeight - 1 ) / ( NewHeight - 1 );
			}
			else
			{
				ky = (double) SrcHeight / NewHeight;
				oy += ( ky - 1.0 ) * 0.5;
			}
		}
		else
		if( ky0 > 0.0 )
		{
			ky = ky0;

			if( ky0 > 1.0 )
			{
				oy += ( ky0 - 1.0 ) * 0.5;
			}
		}
		else
		{
			ky = -ky0;
		}

		if( rfh.update( la, kx ))
		{
			rsh.reset();
			rsv.reset();
		}

		CResizeFilters* rfv; // Pointer to resizing filters for vertical
			// resizing, may equal to "rfh" if the same stepping is in use.

		if( ky == kx )
		{
			rfv = &rfh;
		}
		else
		{
			rfv = &rfv0;

			if( rfv0.update( la, ky ))
			{
				rsv.reset();
			}
		}

		rsh.update( kx, ox, ElCount, SrcWidth, NewWidth, rfh );
		rsv.update( ky, oy, ElCount, SrcHeight, NewHeight, *rfv );

		const int NewWidthE = NewWidth * ElCount;

		if( SrcScanlineSize < 1 )
		{
			SrcScanlineSize = SrcWidth * ElCount;
		}

		// Allocate/resize temporary buffer.

		const size_t FltBufLenNew = (size_t) NewWidthE * (size_t) SrcHeight;

		if( FltBufLenNew > FltBufLen )
		{
			free( FltBuf );
			FltBufLen = FltBufLenNew;
			FltBuf = (float *) memalign( 32, sizeof(float) * FltBufLen );
			CHECK_NOTNULL(FltBuf);
		}

		// Perform horizontal resizing.

		const T* ips = SrcBuf;
		float* op = FltBuf;
		size_t i;

		if( ElCount == 3 )
		{
			for( i = 0; i < SrcHeight; i++ )
			{
				copyScanline3h( ips, rsh, SrcWidth );
				resize3( op, NewWidth, rsh.pos, rfh.KernelLen );
				ips += SrcScanlineSize;
				op += NewWidthE;
			}
		}
		else
		if( ElCount == 1 )
		{
			for( i = 0; i < SrcHeight; i++ )
			{
				copyScanline1h( ips, rsh, SrcWidth );
				resize1( op, NewWidth, rsh.pos, rfh.KernelLen );
				ips += SrcScanlineSize;
				op += NewWidthE;
			}
		}
		else
		if( ElCount == 4 )
		{
			for( i = 0; i < SrcHeight; i++ )
			{
				copyScanline4h( ips, rsh, SrcWidth );
				resize4( op, NewWidth, rsh.pos, rfh.KernelLen );
				ips += SrcScanlineSize;
				op += NewWidthE;
			}
		}
		else
		if( ElCount == 2 )
		{
			for( i = 0; i < SrcHeight; i++ )
			{
				copyScanline2h( ips, rsh, SrcWidth );
				resize2( op, NewWidth, rsh.pos, rfh.KernelLen );
				ips += SrcScanlineSize;
				op += NewWidthE;
			}
		}

		// Perform vertical resizing.

		const int spvlennew = NewHeight * ElCount;

		if( spvlennew > spvlen )
		{
			free( spv );
			spvlen = spvlennew;
			spv = (float *) memalign( 32, sizeof(float) * spvlen );
		}

		const bool IsIOFloat = ( (T) 0.25 != 0 );
		const int Clamp = ( sizeof( T ) == 1 ? 255 : 65535 );
		const float* ip = FltBuf;
		T* opd = NewBuf;

		if( ElCount == 3 )
		{
			for( i = 0; i < NewWidth; i++ )
			{
				copyScanline3v( ip, rsv, SrcHeight, NewWidthE );
				resize3( spv, NewHeight, rsv.pos, rfv -> KernelLen );
				copyOutput3( spv, opd, NewHeight, NewWidthE, IsIOFloat,
					Clamp );

				ip += 3;
				opd += 3;
			}
		}
		else
		if( ElCount == 1 )
		{
			for( i = 0; i < NewWidth; i++ )
			{
				copyScanline1v( ip, rsv, SrcHeight, NewWidthE );
				resize1( spv, NewHeight, rsv.pos, rfv -> KernelLen );
				copyOutput1( spv, opd, NewHeight, NewWidthE, IsIOFloat,
					Clamp );

				ip++;
				opd++;
			}
		}
		else
		if( ElCount == 4 )
		{
			for( i = 0; i < NewWidth; i++ )
			{
				copyScanline4v( ip, rsv, SrcHeight, NewWidthE );
				resize4( spv, NewHeight, rsv.pos, rfv -> KernelLen );
				copyOutput4( spv, opd, NewHeight, NewWidthE, IsIOFloat,
					Clamp );

				ip += 4;
				opd += 4;
			}
		}
		else
		if( ElCount == 2 )
		{
			for( i = 0; i < NewWidth; i++ )
			{
				copyScanline2v( ip, rsv, SrcHeight, NewWidthE );
				resize2( spv, NewHeight, rsv.pos, rfv -> KernelLen );
				copyOutput2( spv, opd, NewHeight, NewWidthE, IsIOFloat,
					Clamp );

				ip += 2;
				opd += 2;
			}
		}
	}

protected:
	float* FltBuf; ///< Intermediate resizing buffer.
		///<
	size_t FltBufLen; ///< Intermediate resizing buffer length.
		///<
	float* spv; ///< Scanline buffer for vertical resizing.
		///<
	unsigned spvlen; ///< Length of "spv".
		///<

	/**
	 * Function rounds a value and applies clamping.
	 *
	 * @param v Value to round and clamp.
	 * @param Clamp High clamp level, low level is 0.
	 */
	static int roundclamp( const float v, const int Clamp )
#define roundclamp(V, CLAMP) ((CLAMP)==255?MAX(0, MIN(255, lrintf(V))):(roundclamp)(V, CLAMP))
	{
		if( Clamp == 255 ) {
			return MAX(0, MIN(255, lrintf(v)));
		} else {
			if( v <= 0.0f )
			{
				return( 0 );
			}
			const int vr = (int) ( v + 0.5f );
			if( vr > Clamp )
			{
				return( Clamp );
			}
			return( vr );
		}
	}

	/**
	 * Function performs final output of the resized scanline data to the
	 * destination image buffer. Variants for 1-4-channel image.
	 *
	 * @param ip Input resized scanline.
	 * @param op Output image buffer.
	 * @param l Pixel count.
	 * @param opinc "op" increment, should account ElCount.
	 * @param IsIOFloat "True" if float output and no clamping is necessary.
	 * @param Clamp Clamp high level, used if IsIOFloat is "false".
	 */

	template< class T >
	static void copyOutput1( const float* ip, T* op, int l, const int opinc,
		const bool IsIOFloat, const int Clamp )
	{
		if( IsIOFloat )
		{
			while( l > 0 )
			{
				op[ 0 ] = (T) ip[ 0 ];
				ip++;
				op += opinc;
				l--;
			}
		}
		else
		{
			while( l > 0 )
			{
				op[ 0 ] = (T) roundclamp( ip[ 0 ], Clamp );
				ip++;
				op += opinc;
				l--;
			}
		}
	}

	template< class T >
	static void copyOutput2( const float* ip, T* op, int l, const int opinc,
		const bool IsIOFloat, const int Clamp )
	{
		if( IsIOFloat )
		{
			while( l > 0 )
			{
				op[ 0 ] = (T) ip[ 0 ];
				op[ 1 ] = (T) ip[ 1 ];
				ip += 2;
				op += opinc;
				l--;
			}
		}
		else
		{
			while( l > 0 )
			{
				op[ 0 ] = (T) roundclamp( ip[ 0 ], Clamp );
				op[ 1 ] = (T) roundclamp( ip[ 1 ], Clamp );
				ip += 2;
				op += opinc;
				l--;
			}
		}
	}

	template< class T >
	static void copyOutput3( const float* ip, T* op, int l, const int opinc,
		const bool IsIOFloat, const int Clamp )
	{
		if( IsIOFloat )
		{
			while( l > 0 )
			{
				op[ 0 ] = (T) ip[ 0 ];
				op[ 1 ] = (T) ip[ 1 ];
				op[ 2 ] = (T) ip[ 2 ];
				ip += 3;
				op += opinc;
				l--;
			}
		}
		else
		{
			while( l > 0 )
			{
				op[ 0 ] = (T) roundclamp( ip[ 0 ], Clamp );
				op[ 1 ] = (T) roundclamp( ip[ 1 ], Clamp );
				op[ 2 ] = (T) roundclamp( ip[ 2 ], Clamp );
				ip += 3;
				op += opinc;
				l--;
			}
		}
	}

	template< class T >
	static void copyOutput4( const float* ip, T* op, int l, const int opinc,
		const bool IsIOFloat, const int Clamp )
	{
		if( IsIOFloat )
		{
			while( l > 0 )
			{
				op[ 0 ] = (T) ip[ 0 ];
				op[ 1 ] = (T) ip[ 1 ];
				op[ 2 ] = (T) ip[ 2 ];
				op[ 3 ] = (T) ip[ 3 ];
				ip += 4;
				op += opinc;
				l--;
			}
		}
		else
		{
			while( l > 0 )
			{
				op[ 0 ] = (T) roundclamp( ip[ 0 ], Clamp );
				op[ 1 ] = (T) roundclamp( ip[ 1 ], Clamp );
				op[ 2 ] = (T) roundclamp( ip[ 2 ], Clamp );
				op[ 3 ] = (T) roundclamp( ip[ 3 ], Clamp );
				ip += 4;
				op += opinc;
				l--;
			}
		}
	}

	class CResizeScanline;

	/**
	 * Class implements fractional delay filter bank calculation.
	 */

	class CResizeFilters
	{
		friend class CResizeScanline;

	public:
		int KernelLen; ///< Resampling filter kernel length, taps. Available
			///< after the update() function call.
			///<

		CResizeFilters()
			: FilterBuf( NULL )
			, Filters( NULL )
			, Prevla( -1.0 )
			, Prevk( -1.0 )
			, FilterBufLen( 0 )
			, FiltersLen( 0 )
		{
		}

		~CResizeFilters()
		{
			free( FilterBuf );
			delete[] Filters;
		}

		/**
		 * Function updates the resizing filter bank.
		 *
		 * @param la Lanczos "a" parameter value.
		 * @param k Resizing step.
		 * @return "True" if update occured and resizing positions should be
		 * updated unconditionally.
		 */

		bool update( const double la, const double k )
		{
			if( la == Prevla && k == Prevk )
			{
				return( false );
			}

			Prevla = la;
			Prevk = k;

			NormFreq = ( k <= 1.0 ? 1.0 : 1.0 / k );
			Freq = LANCIR_PI * NormFreq;

			if( Freq > LANCIR_PI )
			{
				Freq = LANCIR_PI;
			}

			FreqA = LANCIR_PI * NormFreq / la;
			Len2 = la / NormFreq;
			fl2 = (int) ceil( Len2 );
			KernelLen = fl2 + fl2;

			FracCount = 607; // For 8-bit precision.
			FracFill = 0;

			const int FilterBufLenNew = FracCount * KernelLen;

			if( FilterBufLenNew > FilterBufLen )
			{
				free( FilterBuf );
				FilterBufLen = FilterBufLenNew;
				FilterBuf = (float *) memalign( 32, sizeof(float) * FilterBufLen );
				CHECK_NOTNULL(FilterBuf);
			}

			if( FracCount > FiltersLen )
			{
				delete[] Filters;
				FiltersLen = FracCount;
				Filters = new float*[ FiltersLen ];
			}

			memset( Filters, 0, FracCount * sizeof( float* ));

			return( true );
		}

		/**
		 * Function returns filter at the specified fractional offset. This
		 * function can only be called before the prior update() function
		 * call.
		 *
		 * @param x Fractional offset, [0; 1).
		 */

		float* getFilter( const double x )
		{
			const int Frac = (int) floor( x * FracCount );

			if( Filters[ Frac ] == NULL )
			{
				Filters[ Frac ] = FilterBuf + FracFill * KernelLen;
				FracFill++;
				makeFilter( 1.0 - (double) Frac / FracCount, Filters[ Frac ]);
				normalizeFilter( Filters[ Frac ]);
			}

			return( Filters[ Frac ]);
		}

	protected:
		double NormFreq; ///< Normalized frequency of the filter.
			///<
		double Freq; ///< Circular frequency of the filter.
			///<
		double FreqA; ///< Circular frequency of the window function.
			///<
		double Len2; ///< Half resampling filter length, unrounded.
			///<
		int fl2; ///< Half resampling length, integer.
			///<
		int FracCount; ///< The number of fractional positions for which
			///< filters are created.
			///<
		int FracFill; ///< The number of fractional positions filled in the
			///< filter buffer.
			///<
		float* FilterBuf; ///< Buffer that holds all filters.
			///<
		float** Filters; ///< Fractional delay filters for all positions.
			///< Filter pointers equal NULL if filter was not yet created.
			///<
		double Prevla; ///< Previous "la".
			///<
		double Prevk; ///< Previous "k".
			///<
		int FilterBufLen; ///< Allocated length of FilterBuf in elements.
			///<
		int FiltersLen; ///< Allocated length of Filters in elements.
			///<

		/**
		 * @brief Sine signal generator class.
		 *
		 * Class implements sine signal generator without biasing, with
		 * constructor-based initalization only. This generator uses
		 * oscillator instead of "sin" function.
		 */

		class CSinGen
		{
		public:
			/**
			 * Constructor initializes *this sine signal generator.
			 *
			 * @param si Sine function increment, in radians.
			 * @param ph Starting phase, in radians. Add 0.5 * LANCIR_PI for
			 * cosine function.
			 * @param g Gain value.
			 */

			CSinGen( const double si, const double ph, const double g = 1.0 )
				: svalue1( sin( ph ) * g )
				, svalue2( sin( ph - si ) * g )
				, sincr( 2.0 * cos( si ))
			{
			}

			/**
			 * @return The next value of the sine function, without biasing.
			 */

			double generate()
			{
				const double res = svalue1;

				svalue1 = sincr * res - svalue2;
				svalue2 = res;

				return( res );
			}

		private:
			double svalue1; ///< Current sine value.
				///<
			double svalue2; ///< Previous sine value.
				///<
			double sincr; ///< Sine value increment.
				///<
		};

		/**
		 * Function creates filter for the specified fractional delay. The
		 * update() function should be called prior to calling this function.
		 *
		 * @param FracDelay Fractional delay, 0 to 1, inclusive.
		 * @param[out] Output filter buffer.
		 * @tparam T Output buffer type.
		 */

		template< class T >
		void makeFilter( const double FracDelay, T* op ) const
		{
			CSinGen f( Freq, Freq * ( FracDelay - fl2 ));
			CSinGen fw( FreqA, FreqA * ( FracDelay - fl2 ), Len2 );

			int t = -fl2;

			if( t + FracDelay < -Len2 )
			{
				f.generate();
				fw.generate();
				*op = (T) 0.0;
				op++;
				t++;
			}

			int mt = ( FracDelay >= 1.0 - 1e-13 && FracDelay <= 1.0 + 1e-13 ?
				-1 : 0 );

			while( t < mt )
			{
				double ut = ( t + FracDelay ) * LANCIR_PI;
				*op = (T) ( f.generate() * fw.generate() / ( ut * ut ));
				op++;
				t++;
			}

			double ut = t + FracDelay;

			if( fabs( ut ) <= 1e-13 )
			{
				*op = (T) NormFreq;
				f.generate();
				fw.generate();
			}
			else
			{
				ut *= LANCIR_PI;
				*op = (T) ( f.generate() * fw.generate() / ( ut * ut ));
			}

			mt = fl2 - 2;

			while( t < mt )
			{
				op++;
				t++;
				ut = ( t + FracDelay ) * LANCIR_PI;
				*op = (T) ( f.generate() * fw.generate() / ( ut * ut ));
			}

			op++;
			t++;
			ut = t + FracDelay;

			if( ut > Len2 )
			{
				*op = (T) 0.0;
			}
			else
			{
				ut *= LANCIR_PI;
				*op = (T) ( f.generate() * fw.generate() / ( ut * ut ));
			}
		}

		/**
		 * Function normalizes the specified filter so that it has unity gain
		 * at DC.
		 *
		 * @param p Filter buffer pointer.
		 * @tparam T Filter buffer type.
		 */

		template< class T >
		void normalizeFilter( T* const p ) const
		{
			double s = 0.0;
			size_t i;

			for( i = 0; i < KernelLen; i++ )
			{
				s += p[ i ];
			}

			s = 1.0 / s;

			for( i = 0; i < KernelLen; i++ )
			{
				p[ i ] = (T) ( p[ i ] * s );
			}
		}
	};

	/**
	 * Structure defines source scanline positioning and filters for each
	 * destination pixel.
	 */

	struct CResizePos
	{
		const float* ip; ///< Source image pixel pointer.
			///<
		float* flt; ///< Fractional delay filter.
			///<
	};

	/**
	 * Class contains resizing positioning and a temporary scanline buffer,
	 * prepares source scanline positions for resize filtering.
	 */

	class CResizeScanline
	{
	public:
		int padl; ///< Left-padding (in pixels) required for source scanline.
			///< Available after the update() function call.
			///<
		int padr; ///< Right-padding (in pixels) required for source scanline.
			///< Available after the update() function call.
			///<
		float* sp; ///< Source scanline buffer, with "padl" and "padr"
			///< padding.
			///<
		CResizePos* pos; ///< Source scanline pointers (point to "sp")
			///< and filters for each destination pixel position. Available
			///< after the update() function call.
			///<

		CResizeScanline()
			: sp( NULL )
			, pos( NULL )
			, PrevSrcLen( -1 )
			, PrevDstLen( -1 )
			, Prevk( 0.0 )
			, Prevo( 0.0 )
			, PrevElCount( 0 )
			, splen( 0 )
			, poslen( 0 )
		{
		}

		~CResizeScanline()
		{
			free( sp );
			delete[] pos;
		}

		/**
		 * Function "resets" *this object so that the next update() call fully
		 * updates the position buffer. Reset is necessary if the filter
		 * object was updated.
		 */

		void reset()
		{
			PrevSrcLen = -1;
		}

		/**
		 * Function updates resizing positions, updates "padl", "padr" and
		 * "pos" buffer.
		 *
		 * @param k Resizing step.
		 * @param o0 Initial source image offset.
		 * @param SrcLen Source image scanline length, used to create a
		 * scanline buffer without length pre-calculation.
		 * @param DstLen Destination image scanline length.
		 * @param rf Resizing filters object.
		 */

		void update( const double k, const double o0, const int ElCount,
			const int SrcLen, const size_t DstLen, CResizeFilters& rf )
		{
			if( SrcLen == PrevSrcLen && DstLen == PrevDstLen &&
				k == Prevk && o0 == Prevo && ElCount == PrevElCount )
			{
				return;
			}

			PrevSrcLen = SrcLen;
			PrevDstLen = DstLen;
			Prevk = k;
			Prevo = o0;
			PrevElCount = ElCount;

			const int fl2m1 = rf.fl2 - 1;
			padl = fl2m1 - (int) floor( o0 );

			if( padl < 0 )
			{
				padl = 0;
			}

			padr = (int) floor( o0 + k * ( DstLen - 1 )) + rf.fl2 + 1 -
				SrcLen;

			if( padr < 0 )
			{
				padr = 0;
			}

			const int splennew = ( padl + SrcLen + padr ) * ElCount;

			if( splennew > splen )
			{
				free( sp );
				splen = splennew;
				sp = (float *) memalign( 32, sizeof(float) * splen );
				CHECK_NOTNULL(sp);
			}

			if( DstLen > poslen )
			{
				delete[] pos;
				poslen = DstLen;
				pos = new CResizePos[ poslen ];
			}

			const float* const spo = sp + ( padl - fl2m1 ) * ElCount;
			size_t i;

			for( i = 0; i < DstLen; i++ )
			{
				const double o = o0 + k * i;
				const int ix = (int) floor( o );
				pos[ i ].ip = spo + ix * ElCount;
				pos[ i ].flt = rf.getFilter( o - ix );
			}
		}

	protected:
		int PrevSrcLen; ///< Previous SrcLen.
			///<
		int PrevDstLen; ///< Previous DstLen.
			///<
		double Prevk; ///< Previous "k".
			///<
		double Prevo; ///< Previous "o".
			///<
		int PrevElCount; ///< Previous pixel element count.
			///<
		int splen; ///< Allocated "sp" buffer length.
			///<
		int poslen; ///< Allocated "pos" buffer length.
			///<
	};

	CResizeFilters rfh; ///< Resizing filters for horizontal resizing.
		///<
	CResizeFilters rfv0; ///< Resizing filters for vertical resizing (may not
		///< be in use).
		///<
	CResizeScanline rsh; ///< Horizontal resize scanline.
		///<
	CResizeScanline rsv; ///< Vertical resize scanline.
		///<

	/**
	 * Function copies scanline from the source buffer in its native format
	 * to internal scanline buffer, in preparation for horizontal resizing.
	 * Variants for 1-4-channel images.
	 *
	 * @param ip Source scanline buffer.
	 * @param rs Scanline resizing positions object.
	 * @param l Source scanline length, in pixels.
	 * @param ipinc "ip" increment per pixel.
	 */

	template< class T >
	static void copyScanline1h( const T* ip, CResizeScanline& rs, const int l )
	{
		float* op = rs.sp;
		int i;

		DCHECK_ALIGNED(32, op);

		for( i = 0; i < rs.padl; i++ )
		{
			op[ 0 ] = ip[ 0 ];
			op++;
		}

		for( i = 0; i < l - 1; i++ )
		{
			op[ 0 ] = ip[ 0 ];
			ip++;
			op++;
		}

		for( i = 0; i <= rs.padr; i++ )
		{
			op[ 0 ] = ip[ 0 ];
			op++;
		}
	}

	template< class T >
	static void copyScanline2h( const T* ip, CResizeScanline& rs, const int l )
	{
		float* op = rs.sp;
		int i;

		for( i = 0; i < rs.padl; i++ )
		{
			op[ 0 ] = ip[ 0 ];
			op[ 1 ] = ip[ 1 ];
			op += 2;
		}

		for( i = 0; i < l - 1; i++ )
		{
			op[ 0 ] = ip[ 0 ];
			op[ 1 ] = ip[ 1 ];
			ip += 2;
			op += 2;
		}

		for( i = 0; i <= rs.padr; i++ )
		{
			op[ 0 ] = ip[ 0 ];
			op[ 1 ] = ip[ 1 ];
			op += 2;
		}
	}

	template< class T >
	static void copyScanline3h( const T* ip, CResizeScanline& rs, const int l )
	{
		float* op = rs.sp;
		int i;

		for( i = 0; i < rs.padl; i++ )
		{
			op[ 0 ] = ip[ 0 ];
			op[ 1 ] = ip[ 1 ];
			op[ 2 ] = ip[ 2 ];
			op += 3;
		}

		for( i = 0; i < l - 1; i++ )
		{
			op[ 0 ] = ip[ 0 ];
			op[ 1 ] = ip[ 1 ];
			op[ 2 ] = ip[ 2 ];
			ip += 3;
			op += 3;
		}

		for( i = 0; i <= rs.padr; i++ )
		{
			op[ 0 ] = ip[ 0 ];
			op[ 1 ] = ip[ 1 ];
			op[ 2 ] = ip[ 2 ];
			op += 3;
		}
	}

	template< class T >
	static void copyScanline4h( const T* ip, CResizeScanline& rs, const size_t l )
	{
		float* op = rs.sp;
		size_t i;

		for( i = 0; i < rs.padl; i++ )
		{
			op[ 0 ] = ip[ 0 ];
			op[ 1 ] = ip[ 1 ];
			op[ 2 ] = ip[ 2 ];
			op[ 3 ] = ip[ 3 ];
			op += 4;
		}

		for( i = 0; i < l - 1; i++ )
		{
			op[ 0 ] = ip[ 0 ];
			op[ 1 ] = ip[ 1 ];
			op[ 2 ] = ip[ 2 ];
			op[ 3 ] = ip[ 3 ];
			ip += 4;
			op += 4;
		}

		for( i = 0; i <= rs.padr; i++ )
		{
			op[ 0 ] = ip[ 0 ];
			op[ 1 ] = ip[ 1 ];
			op[ 2 ] = ip[ 2 ];
			op[ 3 ] = ip[ 3 ];
			op += 4;
		}
	}

	/**
	 * Function copies scanline from the source buffer in its native format
	 * to internal scanline buffer, in preparation for vertical resizing.
	 * Variants for 1-4-channel images.
	 *
	 * @param ip Source scanline buffer.
	 * @param rs Scanline resizing positions object.
	 * @param l Source scanline length, in pixels.
	 * @param ipinc "ip" increment per pixel.
	 */

	template< class T >
	static void copyScanline1v( const T* ip, CResizeScanline& rs, const int l,
		const int ipinc )
	{
		float* op = rs.sp;
		int i;

		DCHECK_ALIGNED(32, op);

		for( i = 0; i < rs.padl; i++ )
		{
			op[ 0 ] = ip[ 0 ];
			op++;
		}

		for( i = 0; i < l - 1; i++ )
		{
			op[ 0 ] = ip[ 0 ];
			ip += ipinc;
			op++;
		}

		for( i = 0; i <= rs.padr; i++ )
		{
			op[ 0 ] = ip[ 0 ];
			op++;
		}
	}

	template< class T >
	static void copyScanline2v( const T* ip, CResizeScanline& rs, const int l,
		const int ipinc )
	{
		float* op = rs.sp;
		int i;

		for( i = 0; i < rs.padl; i++ )
		{
			op[ 0 ] = ip[ 0 ];
			op[ 1 ] = ip[ 1 ];
			op += 2;
		}

		for( i = 0; i < l - 1; i++ )
		{
			op[ 0 ] = ip[ 0 ];
			op[ 1 ] = ip[ 1 ];
			ip += ipinc;
			op += 2;
		}

		for( i = 0; i <= rs.padr; i++ )
		{
			op[ 0 ] = ip[ 0 ];
			op[ 1 ] = ip[ 1 ];
			op += 2;
		}
	}

	template< class T >
	static void copyScanline3v( const T* ip, CResizeScanline& rs, const int l,
		const int ipinc )
	{
		float* op = rs.sp;
		int i;

		for( i = 0; i < rs.padl; i++ )
		{
			op[ 0 ] = ip[ 0 ];
			op[ 1 ] = ip[ 1 ];
			op[ 2 ] = ip[ 2 ];
			op += 3;
		}

		for( i = 0; i < l - 1; i++ )
		{
			op[ 0 ] = ip[ 0 ];
			op[ 1 ] = ip[ 1 ];
			op[ 2 ] = ip[ 2 ];
			ip += ipinc;
			op += 3;
		}

		for( i = 0; i <= rs.padr; i++ )
		{
			op[ 0 ] = ip[ 0 ];
			op[ 1 ] = ip[ 1 ];
			op[ 2 ] = ip[ 2 ];
			op += 3;
		}
	}

	template< class T >
	static void copyScanline4v( const T* ip, CResizeScanline& rs, const size_t l,
		const int ipinc )
	{
		float* op = rs.sp;
		size_t i;

		for( i = 0; i < rs.padl; i++ )
		{
			op[ 0 ] = ip[ 0 ];
			op[ 1 ] = ip[ 1 ];
			op[ 2 ] = ip[ 2 ];
			op[ 3 ] = ip[ 3 ];
			op += 4;
		}

		for( i = 0; i < l - 1; i++ )
		{
			op[ 0 ] = ip[ 0 ];
			op[ 1 ] = ip[ 1 ];
			op[ 2 ] = ip[ 2 ];
			op[ 3 ] = ip[ 3 ];
			ip += ipinc;
			op += 4;
		}

		for( i = 0; i <= rs.padr; i++ )
		{
			op[ 0 ] = ip[ 0 ];
			op[ 1 ] = ip[ 1 ];
			op[ 2 ] = ip[ 2 ];
			op[ 3 ] = ip[ 3 ];
			op += 4;
		}
	}

	#define LANCIR_LF_PRE \
			CResizePos* const rpe = rp + DstLen; \
			while( rp < rpe ) \
			{ \
				const float* ip = rp -> ip; \
				const float* const flt = rp -> flt;

	#define LANCIR_LF_POST \
				rp++; \
			}

	static void resize1_kl6( float* op, int DstLen, CResizePos* rp )
	{
		LANCIR_LF_PRE
		op[ 0 ] =
			flt[ 0 ] * ip[ 0 ] +
			flt[ 1 ] * ip[ 1 ] +
			flt[ 2 ] * ip[ 2 ] +
			flt[ 3 ] * ip[ 3 ] +
			flt[ 4 ] * ip[ 4 ] +
			flt[ 5 ] * ip[ 5 ];
		op++;
		LANCIR_LF_POST
	}

	static void resize1_kln( float* op, int DstLen, CResizePos* rp, const int kl )
	{
		LANCIR_LF_PRE
		float sum = 0.0;
		int i;
		for( i = 0; i < kl; i++ )
		{
			sum += flt[ i ] * ip[ i ];
		}
		op[ 0 ] = sum;
		op++;
		LANCIR_LF_POST
	}

	/**
	 * Function performs internal scanline resizing. Variants for 1-4-channel
	 * images.
	 *
	 * @param op Destination buffer.
	 * @param DstLen Destination length, in pixels.
	 * @param rp Resizing positions and filters.
	 * @param kl Filter kernel length, in taps.
	 */
	static void resize1( float* op, int DstLen, CResizePos* rp, const int kl )
	{
		if( kl == 6 )
		{
			resize1_kl6( op, DstLen, rp );
		}
		else
		{
			resize1_kln( op, DstLen, rp, kl );
		}
	}

	static void resize2( float* op, int DstLen, CResizePos* rp, const int kl )
	{
		if( kl == 6 )
		{
			LANCIR_LF_PRE
			op[ 0 ] =
				flt[ 0 ] * ip[ 0 ] +
				flt[ 1 ] * ip[ 2 ] +
				flt[ 2 ] * ip[ 4 ] +
				flt[ 3 ] * ip[ 6 ] +
				flt[ 4 ] * ip[ 8 ] +
				flt[ 5 ] * ip[ 10 ];

			op[ 1 ] =
				flt[ 0 ] * ip[ 1 ] +
				flt[ 1 ] * ip[ 3 ] +
				flt[ 2 ] * ip[ 5 ] +
				flt[ 3 ] * ip[ 7 ] +
				flt[ 4 ] * ip[ 9 ] +
				flt[ 5 ] * ip[ 11 ];

			op += 2;
			LANCIR_LF_POST
		}
		else
		{
			LANCIR_LF_PRE
			float sum[ 2 ];
			sum[ 0 ] = 0.0;
			sum[ 1 ] = 0.0;
			int i;

			for( i = 0; i < kl; i++ )
			{
				const float xx = flt[ i ];
				sum[ 0 ] += xx * ip[ 0 ];
				sum[ 1 ] += xx * ip[ 1 ];
				ip += 2;
			}

			op[ 0 ] = sum[ 0 ];
			op[ 1 ] = sum[ 1 ];
			op += 2;
			LANCIR_LF_POST
		}
	}

	static void resize3( float* op, int DstLen, CResizePos* rp, const int kl )
	{
		if( kl == 6 )
		{
			LANCIR_LF_PRE
			op[ 0 ] =
				flt[ 0 ] * ip[ 0 ] +
				flt[ 1 ] * ip[ 3 ] +
				flt[ 2 ] * ip[ 6 ] +
				flt[ 3 ] * ip[ 9 ] +
				flt[ 4 ] * ip[ 12 ] +
				flt[ 5 ] * ip[ 15 ];

			op[ 1 ] =
				flt[ 0 ] * ip[ 1 ] +
				flt[ 1 ] * ip[ 4 ] +
				flt[ 2 ] * ip[ 7 ] +
				flt[ 3 ] * ip[ 10 ] +
				flt[ 4 ] * ip[ 13 ] +
				flt[ 5 ] * ip[ 16 ];

			op[ 2 ] =
				flt[ 0 ] * ip[ 2 ] +
				flt[ 1 ] * ip[ 5 ] +
				flt[ 2 ] * ip[ 8 ] +
				flt[ 3 ] * ip[ 11 ] +
				flt[ 4 ] * ip[ 14 ] +
				flt[ 5 ] * ip[ 17 ];

			op += 3;
			LANCIR_LF_POST
		}
		else
		{
			LANCIR_LF_PRE
			float sum[ 3 ];
			sum[ 0 ] = 0.0;
			sum[ 1 ] = 0.0;
			sum[ 2 ] = 0.0;
			int i;

			for( i = 0; i < kl; i++ )
			{
				const float xx = flt[ i ];
				sum[ 0 ] += xx * ip[ 0 ];
				sum[ 1 ] += xx * ip[ 1 ];
				sum[ 2 ] += xx * ip[ 2 ];
				ip += 3;
			}

			op[ 0 ] = sum[ 0 ];
			op[ 1 ] = sum[ 1 ];
			op[ 2 ] = sum[ 2 ];
			op += 3;
			LANCIR_LF_POST
		}
	}

	static optimizespeed void resize4( float* op, int DstLen, CResizePos* rp, const size_t kl )
	{
		LANCIR_LF_PRE
		float sum[ 4 ];
		sum[ 0 ] = 0.0;
		sum[ 1 ] = 0.0;
		sum[ 2 ] = 0.0;
		sum[ 3 ] = 0.0;
		size_t i;

		for( i = 0; i < kl; i++ )
		{
			const float xx = flt[ i ];
			sum[ 0 ] += xx * ip[ 0 ];
			sum[ 1 ] += xx * ip[ 1 ];
			sum[ 2 ] += xx * ip[ 2 ];
			sum[ 3 ] += xx * ip[ 3 ];
			ip += 4;
		}

		op[ 0 ] = sum[ 0 ];
		op[ 1 ] = sum[ 1 ];
		op[ 2 ] = sum[ 2 ];
		op[ 3 ] = sum[ 3 ];
		op += 4;
		LANCIR_LF_POST
	}

	#undef LANCIR_LF_PRE
	#undef LANCIR_LF_POST

	static relegated nooptimize noinline void handleEmptySrcCornerCase(
		void * const NewBuf, const size_t Size )
	{
		memset( NewBuf, 0, Size );
	}
};

#undef LANCIR_PI

} // namespace avir

#endif // AVIR_CLANCIR_INCLUDED
