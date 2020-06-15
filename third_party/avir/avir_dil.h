/* clang-format off */
//$ nobt
//$ nocpp

/**
 * @file avir_dil.h
 *
 * @brief Inclusion file for de-interleaved image resizing functions.
 *
 * This file includes the "CImageResizerFilterStepDIL" class which implements
 * image resizing functions in de-interleaved mode.
 *
 * AVIR Copyright (c) 2015-2019 Aleksey Vaneev
 */

namespace avir {

/**
 * @brief De-interleaved filtering steps implementation class.
 *
 * This class implements scanline filtering functions in de-interleaved mode.
 * This means that pixels are processed in groups.
 *
 * @tparam fptype Floating point type to use for storing pixel elements.
 * SIMD types cannot be used.
 * @tparam fptypesimd The SIMD type used to store a pack of "fptype" values.
 */

template< class fptype, class fptypesimd >
class CImageResizerFilterStepDIL :
	public CImageResizerFilterStep< fptype, fptype >
{
public:
	using CImageResizerFilterStep< fptype, fptype > :: IsUpsample;
	using CImageResizerFilterStep< fptype, fptype > :: ResampleFactor;
	using CImageResizerFilterStep< fptype, fptype > :: Flt;
	using CImageResizerFilterStep< fptype, fptype > :: FltOrig;
	using CImageResizerFilterStep< fptype, fptype > :: FltLatency;
	using CImageResizerFilterStep< fptype, fptype > :: Vars;
	using CImageResizerFilterStep< fptype, fptype > :: InLen;
	using CImageResizerFilterStep< fptype, fptype > :: InPrefix;
	using CImageResizerFilterStep< fptype, fptype > :: InSuffix;
	using CImageResizerFilterStep< fptype, fptype > :: InElIncr;
	using CImageResizerFilterStep< fptype, fptype > :: OutLen;
	using CImageResizerFilterStep< fptype, fptype > :: OutPrefix;
	using CImageResizerFilterStep< fptype, fptype > :: OutSuffix;
	using CImageResizerFilterStep< fptype, fptype > :: OutElIncr;
	using CImageResizerFilterStep< fptype, fptype > :: PrefixDC;
	using CImageResizerFilterStep< fptype, fptype > :: SuffixDC;
	using CImageResizerFilterStep< fptype, fptype > :: RPosBuf;
	using CImageResizerFilterStep< fptype, fptype > :: FltBank;
	using CImageResizerFilterStep< fptype, fptype > :: EdgePixelCount;

	/**
	 * Function performs "packing" (de-interleaving) of a scanline and type
	 * conversion. If required, the sRGB gamma correction is applied.
	 *
	 * @param ip0 Input scanline, pixel elements interleaved.
	 * @param op0 Output scanline, pixel elements are grouped, "l" elements
	 * apart.
	 * @param l The number of pixels to "pack".
	 */

	template< class Tin >
	void packScanline( const Tin* const ip0, fptype* const op0,
		const int l ) const
	{
		const int ElCount = Vars -> ElCount;
		int j;

		if( !Vars -> UseSRGBGamma )
		{
			for( j = 0; j < ElCount; j++ )
			{
				const Tin* ip = ip0 + j;
				fptype* const op = op0 + j * InElIncr;
				int i;

				for( i = 0; i < l; i++ )
				{
					op[ i ] = (fptype) *ip;
					ip += ElCount;
				}
			}
		}
		else
		{
			const fptype gm = (fptype) Vars -> InGammaMult;

			for( j = 0; j < ElCount; j++ )
			{
				const Tin* ip = ip0 + j;
				fptype* const op = op0 + j * InElIncr;
				int i;

				for( i = 0; i < l; i++ )
				{
					op[ i ] = convertSRGB2Lin( (fptype) *ip * gm );
					ip += ElCount;
				}
			}
		}
	}

	/**
	 * Function applies Linear to sRGB gamma correction to the specified
	 * scanline.
	 *
	 * @param p Scanline.
	 * @param l The number of pixels to de-linearize.
	 * @param Vars0 Image resizing-related variables.
	 */

	static void applySRGBGamma( fptype* const p0, const int l,
		const CImageResizerVars& Vars0 )
	{
		const int ElCount = Vars0.ElCount;
		const fptype gm = (fptype) Vars0.OutGammaMult;
		int j;

		for( j = 0; j < ElCount; j++ )
		{
			fptype* const p = p0 + j * l;
			int i;

			for( i = 0; i < l; i++ )
			{
				p[ i ] = convertLin2SRGB( p[ i ]) * gm;
			}
		}
	}

	/**
	 * Function converts vertical scanline to horizontal scanline. This
	 * function is called by the image resizer when image is resized
	 * vertically. This means that the vertical scanline is stored in the
	 * same format produced by the packScanline() and maintained by other
	 * filtering functions.
	 *
	 * @param ip Input vertical scanline, pixel elements are grouped, SrcLen
	 * elements apart.
	 * @param op Output buffer (temporary buffer used during resizing), pixel
	 * elements are grouped, "l" elements apart.
	 * @param SrcLen The number of pixels in the input scanline, also used to
	 * calculate input buffer increment.
	 * @param SrcIncr Input buffer increment to the next vertical pixel.
	 */

	void convertVtoH( const fptype* ip, fptype* op, const int SrcLen,
		const int SrcIncr ) const
	{
		const int ElCount = Vars -> ElCount;
		const int SrcElIncr = SrcIncr / ElCount;
		const int ips1 = SrcElIncr;
		const int ips2 = SrcElIncr * 2;
		const int ips3 = SrcElIncr * 3;
		const int ops1 = InElIncr;
		const int ops2 = InElIncr * 2;
		const int ops3 = InElIncr * 3;
		int j;

		if( ElCount == 1 )
		{
			for( j = 0; j < SrcLen; j++ )
			{
				op[ 0 ] = ip[ 0 ];
				ip += SrcIncr;
				op++;
			}
		}
		else
		if( ElCount == 4 )
		{
			for( j = 0; j < SrcLen; j++ )
			{
				op[ 0 ] = ip[ 0 ];
				op[ ops1 ] = ip[ ips1 ];
				op[ ops2 ] = ip[ ips2 ];
				op[ ops3 ] = ip[ ips3 ];
				ip += SrcIncr;
				op++;
			}
		}
		else
		if( ElCount == 3 )
		{
			for( j = 0; j < SrcLen; j++ )
			{
				op[ 0 ] = ip[ 0 ];
				op[ ops1 ] = ip[ ips1 ];
				op[ ops2 ] = ip[ ips2 ];
				ip += SrcIncr;
				op++;
			}
		}
		else
		if( ElCount == 2 )
		{
			for( j = 0; j < SrcLen; j++ )
			{
				op[ 0 ] = ip[ 0 ];
				op[ ops1 ] = ip[ ips1 ];
				ip += SrcIncr;
				op++;
			}
		}
	}

	/**
	 * Function performs "unpacking" of a scanline and type conversion
	 * (truncation is used when floating point is converted to integer).
	 * The unpacking function assumes that scanline is stored in the style
	 * produced by the packScanline() function.
	 *
	 * @param ip0 Input scanline, pixel elements are grouped, "l" elements
	 * apart.
	 * @param op0 Output scanline, pixel elements are interleaved.
	 * @param l The number of pixels to "unpack".
	 * @param Vars0 Image resizing-related variables. ElCount is assumed to be
	 * equal to ElCountIO.
	 */

	template< class Tout >
	static void unpackScanline( const fptype* const ip0, Tout* const op0,
		const int l, const CImageResizerVars& Vars0 )
	{
		const int ElCount = Vars0.ElCount;
		int j;

		for( j = 0; j < ElCount; j++ )
		{
			const fptype* const ip = ip0 + j * l;
			Tout* op = op0 + j;
			int i;

			for( i = 0; i < l; i++ )
			{
				*op = (Tout) ip[ i ];
				op += ElCount;
			}
		}
	}

	/**
	 * Function prepares input scanline buffer for *this filtering step.
	 * Left- and right-most pixels are replicated to make sure no buffer
	 * overrun happens. Such approach also allows to bypass any pointer
	 * range checks.
	 *
	 * @param Src Source buffer.
	 */

	void prepareInBuf( fptype* Src ) const
	{
		if( IsUpsample || InPrefix + InSuffix == 0 )
		{
			return;
		}

		int j;

		for( j = 0; j < Vars -> ElCount; j++ )
		{
			replicateArray( Src, 1, Src - InPrefix, InPrefix, 1 );
			fptype* const Src2 = Src + InLen - 1;
			replicateArray( Src2, 1, Src2 + 1, InSuffix, 1 );
			Src += InElIncr;
		}
	}

	/**
	 * Function peforms scanline upsampling with filtering.
	 *
	 * @param Src Source scanline buffer (length = this -> InLen). Source
	 * scanline increment will be equal to ElCount.
	 * @param Dst Destination scanline buffer.
	 */

	void doUpsample( const fptype* Src, fptype* Dst ) const
	{
		const int elalign = Vars -> elalign;
		const int opstep = ResampleFactor;
		const fptype* const f = Flt;
		const int flen = Flt.getCapacity();
		int l;
		int i;
		int j;

		for( j = 0; j < Vars -> ElCount; j++ )
		{
			const fptype* ip = Src;
			fptype* op0 = &Dst[ -OutPrefix ];
			memset( op0, 0, ( OutPrefix + OutLen + OutSuffix ) *
				sizeof( fptype ));

			if( FltOrig.getCapacity() > 0 )
			{
				// Do not perform filtering, only upsample.

				op0 += OutPrefix % ResampleFactor;
				l = OutPrefix / ResampleFactor;

				while( l > 0 )
				{
					op0[ 0 ] = ip[ 0 ];
					op0 += opstep;
					l--;
				}

				l = InLen - 1;

				while( l > 0 )
				{
					op0[ 0 ] = ip[ 0 ];
					op0 += opstep;
					ip++;
					l--;
				}

				l = OutSuffix / ResampleFactor;

				while( l >= 0 )
				{
					op0[ 0 ] = ip[ 0 ];
					op0 += opstep;
					l--;
				}

				Src += InElIncr;
				Dst += OutElIncr;
				continue;
			}

			l = InPrefix;
			fptypesimd ipv = (fptypesimd) ip[ 0 ];

			while( l > 0 )
			{
				for( i = 0; i < flen; i += elalign )
				{
					fptypesimd :: addu( op0 + i,
						fptypesimd :: load( f + i ) * ipv );
				}

				op0 += opstep;
				l--;
			}

			l = InLen - 1;

			while( l > 0 )
			{
				ipv = (fptypesimd) ip[ 0 ];

				for( i = 0; i < flen; i += elalign )
				{
					fptypesimd :: addu( op0 + i,
						fptypesimd :: load( f + i ) * ipv );
				}

				ip++;
				op0 += opstep;
				l--;
			}

			l = InSuffix;
			ipv = (fptypesimd) ip[ 0 ];

			while( l >= 0 )
			{
				for( i = 0; i < flen; i += elalign )
				{
					fptypesimd :: addu( op0 + i,
						fptypesimd :: load( f + i ) * ipv );
				}

				op0 += opstep;
				l--;
			}

			const fptype* dc = SuffixDC;
			l = SuffixDC.getCapacity();

			for( i = 0; i < l; i += elalign )
			{
				fptypesimd :: addu( op0 + i,
					fptypesimd :: load( dc + i ) * ipv );
			}

			ipv = (fptypesimd) Src[ 0 ];
			op0 = Dst - InPrefix * opstep;
			dc = PrefixDC;
			l = PrefixDC.getCapacity();

			for( i = 0; i < l; i += elalign )
			{
				fptypesimd :: addu( op0 + i,
					fptypesimd :: load( dc + i ) * ipv );
			}

			Src += InElIncr;
			Dst += OutElIncr;
		}
	}

	/**
	 * Function peforms scanline filtering with optional downsampling.
	 * Function makes use of the symmetry of the filter.
	 *
	 * @param Src Source scanline buffer (length = this -> InLen). Source
	 * scanline increment will be equal to 1.
	 * @param Dst Destination scanline buffer.
	 * @param DstIncr Destination scanline buffer increment, used for
	 * horizontal or vertical scanline stepping.
	 */

	void doFilter( const fptype* const Src, fptype* Dst,
		const int DstIncr ) const
	{
		const int ElCount = Vars -> ElCount;
		const int elalign = Vars -> elalign;
		const fptype* const f = &Flt[ 0 ];
		const int flen = Flt.getCapacity();
		const int ipstep = ResampleFactor;
		int i;
		int j;

		if( ElCount == 1 )
		{
			const fptype* ip = Src - EdgePixelCount * ipstep - FltLatency;
			fptype* op = Dst;
			int l = OutLen;

			while( l > 0 )
			{
				fptypesimd s = fptypesimd :: load( f ) *
					fptypesimd :: loadu( ip );

				for( i = elalign; i < flen; i += elalign )
				{
					s += fptypesimd :: load( f + i ) *
						fptypesimd :: loadu( ip + i );
				}

				op[ 0 ] = s.hadd();
				op += DstIncr;
				ip += ipstep;
				l--;
			}
		}
		else
		if( DstIncr == 1 )
		{
			for( j = 0; j < ElCount; j++ )
			{
				const fptype* ip = Src - EdgePixelCount * ipstep -
					FltLatency + j * InElIncr;

				fptype* op = Dst + j * OutElIncr;
				int l = OutLen;

				while( l > 0 )
				{
					fptypesimd s = fptypesimd :: load( f ) *
						fptypesimd :: loadu( ip );

					for( i = elalign; i < flen; i += elalign )
					{
						s += fptypesimd :: load( f + i ) *
							fptypesimd :: loadu( ip + i );
					}

					op[ 0 ] = s.hadd();
					op += DstIncr;
					ip += ipstep;
					l--;
				}
			}
		}
		else
		{
			const fptype* ip0 = Src - EdgePixelCount * ipstep - FltLatency;
			fptype* op0 = Dst;
			int l = OutLen;

			while( l > 0 )
			{
				const fptype* ip = ip0;
				fptype* op = op0;

				for( j = 0; j < ElCount; j++ )
				{
					fptypesimd s = fptypesimd :: load( f ) *
						fptypesimd :: loadu( ip );

					for( i = elalign; i < flen; i += elalign )
					{
						s += fptypesimd :: load( f + i ) *
							fptypesimd :: loadu( ip + i );
					}

					op[ 0 ] = s.hadd();
					ip += InElIncr;
					op += OutElIncr;
				}

				ip0 += ipstep;
				op0 += DstIncr;
				l--;
			}
		}
	}

	/**
	 * Function performs resizing of a single scanline. This function does
	 * not "know" about the length of the source scanline buffer. This buffer
	 * should be padded with enough pixels so that ( SrcPos - FilterLenD2 ) is
	 * always >= 0 and ( SrcPos + ( DstLineLen - 1 ) * k + FilterLenD2 + 1 )
	 * does not exceed source scanline's buffer length. SrcLine's increment is
	 * assumed to be equal to 1.
	 *
	 * @param SrcLine Source scanline buffer.
	 * @param DstLine Destination (resized) scanline buffer.
	 * @param DstLineIncr Destination scanline position increment, used for
	 * horizontal or vertical scanline stepping.
	 * @param xx Temporary buffer, of size FltBank -> getFilterLen(), must be
	 * aligned by fpclass :: fpalign.
	 */

	void doResize( const fptype* SrcLine, fptype* DstLine,
		int DstLineIncr, fptype* const xx ) const
	{
		const int IntFltLen = FltBank -> getFilterLen();
		const int ElCount = Vars -> ElCount;
		const int elalign = Vars -> elalign;
		const typename CImageResizerFilterStep< fptype, fptype > ::
			CResizePos* rpos = &(*RPosBuf)[ 0 ];

		int DstLineLen = OutLen;
		int i;
		int j;

#define AVIR_RESIZE_PART1 \
			while( DstLineLen > 0 ) \
			{ \
				const fptypesimd x = (fptypesimd) rpos -> x; \
				const fptype* ftp = rpos -> ftp; \
				const fptype* ftp2 = rpos -> ftp + IntFltLen; \
				const fptype* Src = SrcLine + rpos -> SrcOffs;

#define AVIR_RESIZE_PART1nx \
			while( DstLineLen > 0 ) \
			{ \
				const fptype* ftp = rpos -> ftp; \
				const fptype* Src = SrcLine + rpos -> SrcOffs;

#define AVIR_RESIZE_PART2 \
				DstLine += DstLineIncr; \
				rpos++; \
				DstLineLen--; \
			}

		if( ElCount == 1 )
		{
			if( FltBank -> getOrder() == 1 )
			{
				AVIR_RESIZE_PART1

				fptypesimd sum = ( fptypesimd :: load( ftp ) +
					fptypesimd :: load( ftp2 ) * x ) *
					fptypesimd :: loadu( Src );

				for( i = elalign; i < IntFltLen; i += elalign )
				{
					sum += ( fptypesimd :: load( ftp + i ) +
						fptypesimd :: load( ftp2 + i ) * x ) *
						fptypesimd :: loadu( Src + i );
				}

				DstLine[ 0 ] = sum.hadd();

				AVIR_RESIZE_PART2
			}
			else
			{
				AVIR_RESIZE_PART1nx

				fptypesimd sum = fptypesimd :: load( ftp ) *
					fptypesimd :: loadu( Src );

				for( i = elalign; i < IntFltLen; i += elalign )
				{
					sum += fptypesimd :: load( ftp + i ) *
						fptypesimd :: loadu( Src + i );
				}

				DstLine[ 0 ] = sum.hadd();

				AVIR_RESIZE_PART2
			}
		}
		else
		if( DstLineIncr == 1 )
		{
			// Horizontal-oriented processing, element loop is outer.

			const int SrcIncr = InElIncr;
			const int DstLineElIncr = OutElIncr - DstLineIncr * DstLineLen;

			if( FltBank -> getOrder() == 1 )
			{
				for( j = 0; j < ElCount; j++ )
				{
					AVIR_RESIZE_PART1

					fptypesimd sum = 0.0;

					for( i = 0; i < IntFltLen; i += elalign )
					{
						sum += ( fptypesimd :: load( ftp + i ) +
							fptypesimd :: load( ftp2 + i ) * x ) *
							fptypesimd :: loadu( Src + i );
					}

					DstLine[ 0 ] = sum.hadd();

					AVIR_RESIZE_PART2

					DstLine += DstLineElIncr;
					SrcLine += SrcIncr;
					DstLineLen = OutLen;
					rpos = &(*RPosBuf)[ 0 ];
				}
			}
			else
			{
				for( j = 0; j < ElCount; j++ )
				{
					AVIR_RESIZE_PART1nx

					fptypesimd sum = fptypesimd :: load( ftp ) *
						fptypesimd :: loadu( Src );

					for( i = elalign; i < IntFltLen; i += elalign )
					{
						sum += fptypesimd :: load( ftp + i ) *
							fptypesimd :: loadu( Src + i );
					}

					DstLine[ 0 ] = sum.hadd();

					AVIR_RESIZE_PART2

					DstLine += DstLineElIncr;
					SrcLine += SrcIncr;
					DstLineLen = OutLen;
					rpos = &(*RPosBuf)[ 0 ];
				}
			}
		}
		else
		{
			const int SrcIncr = InElIncr;
			const int DstLineElIncr = OutElIncr;
			DstLineIncr -= DstLineElIncr * ElCount;

			if( FltBank -> getOrder() == 1 )
			{
				AVIR_RESIZE_PART1

				for( i = 0; i < IntFltLen; i += elalign )
				{
					( fptypesimd :: load( ftp + i ) +
						fptypesimd :: load( ftp2 + i ) * x ).store( xx + i );
				}

				for( j = 0; j < ElCount; j++ )
				{
					fptypesimd sum = fptypesimd :: load( xx ) *
						fptypesimd :: loadu( Src );

					for( i = elalign; i < IntFltLen; i += elalign )
					{
						sum += fptypesimd :: load( xx + i ) *
							fptypesimd :: loadu( Src + i );
					}

					DstLine[ 0 ] = sum.hadd();
					DstLine += DstLineElIncr;
					Src += SrcIncr;
				}

				AVIR_RESIZE_PART2
			}
			else
			{
				AVIR_RESIZE_PART1nx

				for( j = 0; j < ElCount; j++ )
				{
					fptypesimd sum = fptypesimd :: load( ftp ) *
						fptypesimd :: loadu( Src );

					for( i = elalign; i < IntFltLen; i += elalign )
					{
						sum += fptypesimd :: load( ftp + i ) *
							fptypesimd :: loadu( Src + i );
					}

					DstLine[ 0 ] = sum.hadd();
					DstLine += DstLineElIncr;
					Src += SrcIncr;
				}

				AVIR_RESIZE_PART2
			}
		}

#undef AVIR_RESIZE_PART2
#undef AVIR_RESIZE_PART1nx
#undef AVIR_RESIZE_PART1
	}
};

/**
 * @brief Image resizer's default de-interleaved dithering class.
 *
 * This class defines an object that performs rounding, clipping and dithering
 * operations over horizontal scanline pixels before scanline is stored in the
 * output buffer.
 *
 * This ditherer implementation uses de-interleaved SIMD algorithm.
 *
 * @tparam fptype Floating point type to use for storing pixel data. SIMD
 * types cannot be used.
 * @tparam fptypesimd The SIMD type used to store a pack of "fptype" values.
 */

template< class fptype, class fptypesimd >
class CImageResizerDithererDefDIL
{
public:
	/**
	 * Function initializes the ditherer object.
	 *
	 * @param aLen Scanline length in pixels to process.
	 * @param aVars Image resizing-related variables.
	 * @param aTrMul Bit-depth truncation multiplier. 1 - no additional
	 * truncation.
	 * @param aPkOut Peak output value allowed.
	 */

	void init( const int aLen, const CImageResizerVars& aVars,
		const double aTrMul, const double aPkOut )
	{
		Len = aLen;
		Vars = &aVars;
		LenE = aLen * Vars -> ElCount;
		TrMul0 = aTrMul;
		PkOut0 = aPkOut;
	}

	/**
	 * @return "True" if dithering is recursive relative to scanlines meaning
	 * multi-threaded execution is not supported by this dithering method.
	 */

	static bool isRecursive()
	{
		return( false );
	}

	/**
	 * Function performs rounding and clipping operations.
	 *
	 * @param ResScanline The buffer containing the final scanline.
	 */

	void dither( fptype* const ResScanline ) const
	{
		const int elalign = Vars -> elalign;
		const fptypesimd c0 = 0.0;
		const fptypesimd PkOut = (fptypesimd) PkOut0;
		int j;

		if( TrMul0 == 1.0 )
		{
			// Optimization - do not perform bit truncation.

			for( j = 0; j < LenE - elalign; j += elalign )
			{
				const fptypesimd z0 = round(
					fptypesimd :: loadu( ResScanline + j ));

				clamp( z0, c0, PkOut ).storeu( ResScanline + j );
			}

			const int lim = LenE - j;
			const fptypesimd z0 = round(
				fptypesimd :: loadu( ResScanline + j, lim ));

			clamp( z0, c0, PkOut ).storeu( ResScanline + j, lim );
		}
		else
		{
			const fptypesimd TrMul = (fptypesimd) TrMul0;

			for( j = 0; j < LenE - elalign; j += elalign )
			{
				const fptypesimd z0 = round(
					fptypesimd :: loadu( ResScanline + j ) / TrMul ) * TrMul;

				clamp( z0, c0, PkOut ).storeu( ResScanline + j );
			}

			const int lim = LenE - j;
			const fptypesimd z0 = round(
				fptypesimd :: loadu( ResScanline + j, lim ) / TrMul ) * TrMul;

			clamp( z0, c0, PkOut ).storeu( ResScanline + j, lim );
		}
	}

protected:
	int Len; ///< Scanline's length in pixels.
		///<
	const CImageResizerVars* Vars; ///< Image resizing-related variables.
		///<
	int LenE; ///< = LenE * ElCount.
		///<
	double TrMul0; ///< Bit-depth truncation multiplier.
		///<
	double PkOut0; ///< Peak output value allowed.
		///<
};

/**
 * @brief Image resizer's error-diffusion dithering class, de-interleaved
 * mode.
 *
 * This ditherer implements error-diffusion dithering which looks good, and
 * whose results are compressed by PNG well.
 *
 * @tparam fptype Floating point type to use for storing pixel data. SIMD
 * types cannot be used.
 * @tparam fptypesimd Processing type, SIMD can be used.
 */

template< class fptype, class fptypesimd >
class CImageResizerDithererErrdDIL
{
public:
	/**
	 * Function initializes the ditherer object.
	 *
	 * @param aLen Scanline length in pixels to process.
	 * @param aVars Image resizing-related variables.
	 * @param aTrMul Bit-depth truncation multiplier. 1 - no additional
	 * truncation.
	 * @param aPkOut Peak output value allowed.
	 */

	void init( const int aLen, const CImageResizerVars& aVars,
		const double aTrMul, const double aPkOut )
	{
		Len = aLen;
		Vars = &aVars;
		LenE = aLen * Vars -> ElCount;
		TrMul0 = aTrMul;
		PkOut0 = aPkOut;

		ResScanlineDith0.alloc( LenE + Vars -> ElCount, sizeof( fptype ));
		ResScanlineDith = ResScanlineDith0 + Vars -> ElCount;
		int i;

		for( i = 0; i < LenE + Vars -> ElCount; i++ )
		{
			ResScanlineDith0[ i ] = 0.0;
		}
	}

	static bool isRecursive()
	{
		return( true );
	}

	void dither( fptype* const ResScanline )
	{
		const int ea = Vars -> elalign;
		const fptypesimd c0 = 0.0;
		const fptypesimd TrMul = (fptypesimd) TrMul0;
		const fptypesimd PkOut = (fptypesimd) PkOut0;
		int j;

		for( j = 0; j < LenE - ea; j += ea )
		{
			fptypesimd :: addu( ResScanline + j,
				fptypesimd :: loadu( ResScanlineDith + j ));

			c0.storeu( ResScanlineDith + j );
		}

		int lim = LenE - j;
		fptypesimd :: addu( ResScanline + j,
			fptypesimd :: loadu( ResScanlineDith + j, lim ), lim );

		c0.storeu( ResScanlineDith + j, lim );

		const int Len1 = Len - 1;
		fptype* rs = ResScanline;
		fptype* rsd = ResScanlineDith;
		int i;

		for( i = 0; i < Vars -> ElCount; i++ )
		{
			for( j = 0; j < Len1; j++ )
			{
				// Perform rounding, noise estimation and saturation.

				fptype* const rsj = rs + j;
				const fptype z0 = round( rsj[ 0 ] / TrMul ) * TrMul;
				const fptype Noise = rsj[ 0 ] - z0;
				rsj[ 0 ] = clamp( z0, (fptype) 0.0, PkOut );

				fptype* const rsdj = rsd + j;
				rsj[ 1 ] += Noise * (fptype) 0.364842;
				rsdj[ -1 ] += Noise * (fptype) 0.207305;
				rsdj[ 0 ] += Noise * (fptype) 0.364842;
				rsdj[ 1 ] += Noise * (fptype) 0.063011;
			}

			// Process the last pixel element in scanline.

			const fptype z1 = round( rs[ Len1 ] / TrMul ) * TrMul;
			const fptype Noise2 = rs[ Len1 ] - z1;
			rs[ Len1 ] = clamp( z1, c0, PkOut );

			rsd[ Len1 - 1 ] += Noise2 * (fptype) 0.207305;
			rsd[ Len1 ] += Noise2 * (fptype) 0.364842;

			rs += Len;
			rsd += Len;
		}
	}

protected:
	int Len; ///< Scanline's length in pixels.
		///<
	const CImageResizerVars* Vars; ///< Image resizing-related variables.
		///<
	int LenE; ///< = LenE * ElCount.
		///<
	double TrMul0; ///< Bit-depth truncation multiplier.
		///<
	double PkOut0; ///< Peak output value allowed.
		///<
	CBuffer< fptype > ResScanlineDith0; ///< Error propagation buffer for
		///< dithering, first pixel unused.
		///<
	fptype* ResScanlineDith; ///< Error propagation buffer pointer which skips
		///< the first ElCount elements.
		///<
};

/**
 * @brief Floating-point processing definition and abstraction class for
 * de-interleaved processing.
 *
 * This class defines several constants and typedefs that point to classes
 * that should be used by the image resizing algorithm. This implementation
 * points to de-interleaved processing classes.
 *
 * @tparam afptype Floating point type to use for storing intermediate data
 * and variables. SIMD types should not be used.
 * @tparam afptypesimd SIMD type used to perform processing.
 * @tparam adith Ditherer class to use during processing.
 */

template< class afptype, class afptypesimd,
	class adith = CImageResizerDithererDefDIL< afptype, afptypesimd > >
class fpclass_def_dil
{
public:
	typedef afptype fptype; ///< Floating-point type to use during processing.
		///<
	typedef afptype fptypeatom; ///< Atomic type "fptype" consists of.
		///<
	static const int fppack = 1; ///< The number of atomic types stored in a
		///< single "fptype" element.
		///<
	static const int fpalign = sizeof( afptypesimd ); ///< Suggested alignment
		///< size in bytes. This is not a required alignment, because image
		///< resizing algorithm cannot be made to have a strictly aligned data
		///< access in all cases (e.g. de-interleaved interpolation cannot
		///< perform aligned accesses).
		///<
	static const int elalign = sizeof( afptypesimd ) / sizeof( afptype ); ///<
		///< Length alignment of arrays of elements. This applies to filters
		///< and intermediate buffers: this constant forces filters and
		///< scanlines to have a length which is a multiple of this value, for
		///< more efficient SIMD implementation.
		///<
	static const int packmode = 1; ///< 0 if interleaved packing, 1 if
		///< de-interleaved.
		///<
	typedef CImageResizerFilterStepDIL< fptype, afptypesimd > CFilterStep; ///<
		///< Filtering step class to use during processing.
		///<
	typedef adith CDitherer; ///< Ditherer class to use during processing.
		///<
};

} // namespace avir
