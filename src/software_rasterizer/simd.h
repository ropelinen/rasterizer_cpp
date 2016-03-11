#ifndef RPLNN_SIMD_H
#define RPLNN_SIMD_H

#include <emmintrin.h>

/* SIMD classes to handle four 32bit values */

/* Integer*/

class simd128i
{
	friend class simd128f;

public:
	simd128i() { data = _mm_setzero_si128(); }
	simd128i(const int32_t val) { data = _mm_set_epi32(val, val, val, val); }
	simd128i(const int32_t i3, const int32_t i2, const int32_t i1, const int32_t i0) { data = _mm_set_epi32(i3, i2, i1, i0); }
	simd128i(const __m128i &data) { this->data = data; }
	~simd128i() { }

	inline void set(const int32_t val) { data = _mm_set_epi32(val, val, val, val); }
	inline void set(const int32_t i3, const int32_t i2, const int32_t i1, const int32_t i0) { data = _mm_set_epi32(i3, i2, i1, i0); }
	inline int movemask() { return _mm_movemask_epi8(data); }
	inline int32_t *get_int_ptr() { return (int32_t *)&data; }
	inline uint32_t *get_uint_ptr() { return (uint32_t *)&data; }

	inline simd128i operator+(const simd128i &rhs) const { return simd128i(_mm_add_epi32(data, rhs.data)); }
	inline simd128i &operator+=(const simd128i &rhs)
	{
		data = _mm_add_epi32(data, rhs.data);
		return *this;
	}

	inline simd128i operator-(const simd128i &rhs) const { return simd128i(_mm_sub_epi32(data, rhs.data)); }
	inline simd128i &operator-=(const simd128i &rhs)
	{
		data = _mm_sub_epi32(data, rhs.data);
		return *this;
	}

	inline simd128i operator*(const simd128i &rhs) const
	{
		/* From http://stackoverflow.com/questions/10500766/sse-multiplication-of-4-32-bit-integers */
		__m128i tmp1 = _mm_mul_epu32(data, rhs.data); /* mul 2,0*/
		__m128i tmp2 = _mm_mul_epu32(_mm_srli_si128(data, 4), _mm_srli_si128(rhs.data, 4)); /* mul 3,1 */
		return simd128i(_mm_unpacklo_epi32(_mm_shuffle_epi32(tmp1, _MM_SHUFFLE(0, 0, 2, 0)), _mm_shuffle_epi32(tmp2, _MM_SHUFFLE(0, 0, 2, 0)))); /* shuffle results to [63..0] and pack */
	}
	inline simd128i &operator*=(const simd128i &rhs)
	{

		/* From http://stackoverflow.com/questions/10500766/sse-multiplication-of-4-32-bit-integers */
		__m128i tmp1 = _mm_mul_epu32(data, rhs.data); /* mul 2,0*/
		__m128i tmp2 = _mm_mul_epu32(_mm_srli_si128(data, 4), _mm_srli_si128(rhs.data, 4)); /* mul 3,1 */
		data = _mm_unpacklo_epi32(_mm_shuffle_epi32(tmp1, _MM_SHUFFLE(0, 0, 2, 0)), _mm_shuffle_epi32(tmp2, _MM_SHUFFLE(0, 0, 2, 0))); /* shuffle results to [63..0] and pack */
		return *this;
	}

	inline simd128i operator&(const simd128i &rhs) const { return simd128i(_mm_and_si128(data, rhs.data)); }
	inline simd128i &operator&=(const simd128i &rhs)
	{
		data = _mm_and_si128(data, rhs.data);
		return *this;
	}

	inline simd128i operator|(const simd128i &rhs) const { return simd128i(_mm_or_si128(data, rhs.data)); }
	inline simd128i &operator|=(const simd128i &rhs)
	{
		data = _mm_or_si128(data, rhs.data);
		return *this;
	}

	inline simd128i operator^(const simd128i &rhs) const { return simd128i(_mm_xor_si128(data, rhs.data)); }
	inline simd128i &operator^=(const simd128i &rhs)
	{
		data = _mm_xor_si128(data, rhs.data);
		return *this;
	}

	inline simd128i operator==(const simd128i &rhs) const
	{
		simd128i ret(_mm_cmpeq_epi32(data, rhs.data));
		return ret;
	}

	inline simd128i operator>(const simd128i &rhs) const
	{
		simd128i ret(_mm_cmpgt_epi32(data, rhs.data));
		return ret;
	}

	inline simd128i operator<(const simd128i &rhs) const
	{
		simd128i ret(_mm_cmplt_epi32(data, rhs.data));
		return ret;
	}

private:
	__m128i data;
};

/* Floating point */

class simd128f
{
public:
	inline simd128f() { data = _mm_setzero_ps(); }
	inline simd128f(const float val) { data = _mm_set_ps(val, val, val, val); }
	inline simd128f(const float a, const float b, const float c, const float d)	{ data = _mm_set_ps(a, b, c, d); }
	inline simd128f(const simd128i &val) { data = _mm_cvtepi32_ps(val.data); }
	inline simd128f(const __m128 &data) { this->data = data; }
	~simd128f() { }

	inline void set(const float val) { data = _mm_set_ps(val, val, val, val); }
	inline void set(const float a, const float b, const float c, const float d) { data = _mm_set_ps(a, b, c, d); }

	friend simd128f min_simd(const simd128f &a, const simd128f &b);
	friend simd128f max_simd(const simd128f &a, const simd128f &b);

	inline operator simd128i() { return simd128i(_mm_cvttps_epi32(data)); }

	inline simd128f operator+(const simd128f &rhs) const { return simd128f(_mm_add_ps(data, rhs.data)); }
	inline simd128f &operator+=(const simd128f &rhs)
	{
		data = _mm_add_ps(data, rhs.data);
		return *this;
	}

	inline simd128f operator-(const simd128f &rhs) const { return simd128f(_mm_sub_ps(data, rhs.data)); }
	inline simd128f &operator-=(const simd128f &rhs)
	{
		data = _mm_sub_ps(data, rhs.data);
		return *this;
	}

	inline simd128f operator*(const simd128f &rhs) const { return simd128f(_mm_mul_ps(data, rhs.data)); }
	inline simd128f &operator*=(const simd128f &rhs)
	{
		data = _mm_mul_ps(data, rhs.data);
		return *this;
	}

	inline simd128f operator/(const simd128f &rhs) const { return simd128f(_mm_div_ps(data, rhs.data)); }
	inline simd128f &operator/=(const simd128f &rhs)
	{
		data = _mm_div_ps(data, rhs.data);
		return *this;
	}

	inline simd128f operator^(const simd128f &rhs) const { return simd128f(_mm_cvtpd_ps(_mm_xor_pd(_mm_cvtps_pd(data), _mm_cvtps_pd(rhs.data)))); }
	inline simd128f &operator^=(const simd128f &rhs)
	{
		data = _mm_cvtpd_ps(_mm_xor_pd(_mm_cvtps_pd(data), _mm_cvtps_pd(rhs.data)));
		return *this;
	}

private:
	__m128 data;
};

inline simd128f min_simd(const simd128f &a, const simd128f &b)
{
	return simd128f(_mm_min_ps(a.data, b.data));
}

inline simd128f max_simd(const simd128f &a, const simd128f &b)
{
	return simd128f(_mm_max_ps(a.data, b.data));
}

#endif /* RPLNN_SIMD_H */