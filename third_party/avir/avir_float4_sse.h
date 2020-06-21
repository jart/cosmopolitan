/* clang-format off */
//$ nobt
//$ nocpp

/**
 * @file avir_float4_sse.h
 *
 * @brief Inclusion file for the "float4" type.
 *
 * This file includes the "float4" SSE-based type used for SIMD variable
 * storage and processing.
 *
 * AVIR Copyright (c) 2015-2019 Aleksey Vaneev
 */

#ifndef AVIR_FLOAT4_SSE_INCLUDED
#define AVIR_FLOAT4_SSE_INCLUDED

#include "third_party/avir/avir.h"
#include "libc/bits/mmintrin.h"
#include "libc/bits/xmmintrin.h"
#include "libc/bits/xmmintrin.h"
#include "libc/bits/xmmintrin.h"
#include "libc/bits/emmintrin.h"

namespace avir {

/**
 * @brief SIMD packed 4-float type.
 *
 * This class implements a packed 4-float type that can be used to perform
 * parallel computation using SIMD instructions on SSE-enabled processors.
 * This class can be used as the "fptype" argument of the avir::fpclass_def
 * class.
 */

class float4
{
public:
	float4()
	{
	}

	float4( const float4& s )
		: value( s.value )
	{
	}

	float4( const __m128 s )
		: value( s )
	{
	}

	float4( const float s )
		: value( _mm_set1_ps( s ))
	{
	}

	float4& operator = ( const float4& s )
	{
		value = s.value;
		return( *this );
	}

	float4& operator = ( const __m128 s )
	{
		value = s;
		return( *this );
	}

	float4& operator = ( const float s )
	{
		value = _mm_set1_ps( s );
		return( *this );
	}

	operator float () const
	{
		return( _mm_cvtss_f32( value ));
	}

	/**
	 * @param p Pointer to memory from where the value should be loaded,
	 * should be 16-byte aligned.
	 * @return float4 value loaded from the specified memory location.
	 */

	static float4 load( const float* const p )
	{
		return( _mm_load_ps( p ));
	}

	/**
	 * @param p Pointer to memory from where the value should be loaded,
	 * may have any alignment.
	 * @return float4 value loaded from the specified memory location.
	 */

	static float4 loadu( const float* const p )
	{
		return( _mm_loadu_ps( p ));
	}

	/**
	 * @param p Pointer to memory from where the value should be loaded,
	 * may have any alignment.
	 * @param lim The maximum number of elements to load, >0.
	 * @return float4 value loaded from the specified memory location, with
	 * elements beyond "lim" set to 0.
	 */

	static float4 loadu( const float* const p, int lim )
	{
		if( lim > 2 )
		{
			if( lim > 3 )
			{
				return( _mm_loadu_ps( p ));
			}
			else
			{
				return( _mm_set_ps( 0.0f, p[ 2 ], p[ 1 ], p[ 0 ]));
			}
		}
		else
		{
			if( lim == 2 )
			{
				return( _mm_set_ps( 0.0f, 0.0f, p[ 1 ], p[ 0 ]));
			}
			else
			{
				return( _mm_load_ss( p ));
			}
		}
	}

	/**
	 * Function stores *this value to the specified memory location.
	 *
	 * @param[out] p Output memory location, should be 16-byte aligned.
	 */

	void store( float* const p ) const
	{
		_mm_store_ps( p, value );
	}

	/**
	 * Function stores *this value to the specified memory location.
	 *
	 * @param[out] p Output memory location, may have any alignment.
	 */

	void storeu( float* const p ) const
	{
		_mm_storeu_ps( p, value );
	}

	/**
	 * Function stores "lim" lower elements of *this value to the specified
	 * memory location.
	 *
	 * @param[out] p Output memory location, may have any alignment.
	 * @param lim The number of lower elements to store, >0.
	 */

	void storeu( float* const p, int lim ) const
	{
		if( lim > 2 )
		{
			if( lim > 3 )
			{
				_mm_storeu_ps( p, value );
			}
			else
			{
				_mm_storel_pi( (__m64*) p, value );
				_mm_store_ss( p + 2, _mm_movehl_ps( value, value ));
			}
		}
		else
		{
			if( lim == 2 )
			{
				_mm_storel_pi( (__m64*) p, value );
			}
			else
			{
				_mm_store_ss( p, value );
			}
		}
	}

	float4& operator += ( const float4& s )
	{
		value = _mm_add_ps( value, s.value );
		return( *this );
	}

	float4& operator -= ( const float4& s )
	{
		value = _mm_sub_ps( value, s.value );
		return( *this );
	}

	float4& operator *= ( const float4& s )
	{
		value = _mm_mul_ps( value, s.value );
		return( *this );
	}

	float4& operator /= ( const float4& s )
	{
		value = _mm_div_ps( value, s.value );
		return( *this );
	}

	float4 operator + ( const float4& s ) const
	{
		return( _mm_add_ps( value, s.value ));
	}

	float4 operator - ( const float4& s ) const
	{
		return( _mm_sub_ps( value, s.value ));
	}

	float4 operator * ( const float4& s ) const
	{
		return( _mm_mul_ps( value, s.value ));
	}

	float4 operator / ( const float4& s ) const
	{
		return( _mm_div_ps( value, s.value ));
	}

	/**
	 * @return Horizontal sum of elements.
	 */

	float hadd() const
	{
		const __m128 v = _mm_add_ps( value, _mm_movehl_ps( value, value ));
		const __m128 res = _mm_add_ss( v, _mm_shuffle_ps( v, v, 1 ));
		return( _mm_cvtss_f32( res ));
	}

	/**
	 * Function performs in-place addition of a value located in memory and
	 * the specified value.
	 *
	 * @param p Pointer to value where addition happens. May be unaligned.
	 * @param v Value to add.
	 */

	static void addu( float* const p, const float4& v )
	{
		( loadu( p ) + v ).storeu( p );
	}

	/**
	 * Function performs in-place addition of a value located in memory and
	 * the specified value. Limited to the specfied number of elements.
	 *
	 * @param p Pointer to value where addition happens. May be unaligned.
	 * @param v Value to add.
	 * @param lim The element number limit, >0.
	 */

	static void addu( float* const p, const float4& v, const int lim )
	{
		( loadu( p, lim ) + v ).storeu( p, lim );
	}

	__m128 value; ///< Packed value of 4 floats.
		///<
};

/**
 * SIMD rounding function, exact result.
 *
 * @param v Value to round.
 * @return Rounded SIMD value.
 */

inline float4 round( const float4& v )
{
	unsigned int prevrm = _MM_GET_ROUNDING_MODE();
	_MM_SET_ROUNDING_MODE( _MM_ROUND_NEAREST );

	const __m128 res = _mm_cvtepi32_ps( _mm_cvtps_epi32( v.value ));

	_MM_SET_ROUNDING_MODE( prevrm );

	return( res );
}

/**
 * SIMD function "clamps" (clips) the specified packed values so that they are
 * not lesser than "minv", and not greater than "maxv".
 *
 * @param Value Value to clamp.
 * @param minv Minimal allowed value.
 * @param maxv Maximal allowed value.
 * @return The clamped value.
 */

inline float4 clamp( const float4& Value, const float4& minv,
	const float4& maxv )
{
	return( _mm_min_ps( _mm_max_ps( Value.value, minv.value ), maxv.value ));
}

typedef fpclass_def< avir :: float4, float > fpclass_float4; ///<
	///< Class that can be used as the "fpclass" template parameter of the
	///< avir::CImageResizer class to perform calculation using default
	///< interleaved algorithm, using SIMD float4 type.
	///<

} // namespace avir

#endif // AVIR_FLOAT4_SSE_INCLUDED
