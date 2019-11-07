//-----------------------------------------------------------------------------------------------
// RawNoise.hpp
//
#pragma once


/////////////////////////////////////////////////////////////////////////////////////////////////
// SquirrelNoise4 - Squirrel's Raw Noise utilities (version 4)
//
// This code is made available under the Creative Commons attribution 3.0 license (CC-BY-3.0 US):
//	Attribution in source code comments (even closed-source/commercial code) is sufficient.
//	License summary and text available at: https://creativecommons.org/licenses/by/3.0/us/
//
// These noise functions were written by Squirrel Eiserloh as a cheap and simple substitute for
//	the [sometimes awful] bit-noise sample code functions commonly found on the web, many of which
//	are hugely biased or terribly patterned, e.g. having bits which are on (or off) 75% or even
//	100% of the time (or are excessively overkill/slow for our needs, such as MD5 or SHA).
//
// Note: This is work in progress, and has not yet been tested thoroughly.  Use at your own risk.
//	Please report any bugs, issues, or bothersome cases to SquirrelEiserloh at gmail.com.
//
// The following functions are all based on a simple bit-noise hash function which returns an
//	unsigned integer containing 32 reasonably-well-scrambled bits, based on a given (signed)
//	integer input parameter (position/index) and [optional] seed.  Kind of like looking up a
//	value in an infinitely large [non-existent] table of previously rolled random numbers.
//
// These functions are deterministic and random-access / order-independent (i.e. state-free),
//	so they are particularly well-suited for use in smoothed/fractal/simplex/Perlin noise
//	functions and out-of-order (or or-demand) procedural content generation (i.e. that mountain
//	village is the same whether you generated it first or last, ahead of time or just now).
//
// The N-dimensional variations simply hash their multidimensional coordinates down to a single
//	32-bit index and then proceed as usual, so while results are not unique they should
//	(hopefully) not seem locally predictable or repetitive.
//
/////////////////////////////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------------------------------
// Raw pseudorandom noise functions (random-access / deterministic).  Basis of all other noise.
//
constexpr unsigned int Get1DNoiseUint(int index, unsigned int seed = 0);
constexpr unsigned int Get2DNoiseUint(int index_x, int index_y, unsigned int seed = 0);
constexpr unsigned int Get3DNoiseUint(int index_x, int index_y, int index_z, unsigned int seed = 0);
constexpr unsigned int Get4DNoiseUint(int index_x, int index_y, int index_z, int index_t, unsigned int seed = 0);

//-----------------------------------------------------------------------------------------------
// Same functions, mapped to floats in [0,1] for convenience.
//
constexpr float Get1DNoiseZeroToOne(int index, unsigned int seed = 0);
constexpr float Get2DNoiseZeroToOne(int index_x, int index_y, unsigned int seed = 0);
constexpr float Get3DNoiseZeroToOne(int index_x, int index_y, int index_z, unsigned int seed = 0);
constexpr float Get4DNoiseZeroToOne(int index_x, int index_y, int index_z, int index_t, unsigned int seed = 0);

//-----------------------------------------------------------------------------------------------
// Same functions, mapped to floats in [-1,1] for convenience.
//
constexpr float Get1DNoiseNegOneToOne(int index, unsigned int seed = 0);
constexpr float Get2DNoiseNegOneToOne(int index_x, int index_y, unsigned int seed = 0);
constexpr float Get3DNoiseNegOneToOne(int index_x, int index_y, int index_z, unsigned int seed = 0);
constexpr float Get4DNoiseNegOneToOne(int index_x, int index_y, int index_z, int index_t, unsigned int seed = 0);


/////////////////////////////////////////////////////////////////////////////////////////////////
// Inline function definitions below
/////////////////////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------------------------
// Fast hash of an int32 into a different (unrecognizable) uint32.
//
// Returns an unsigned integer containing 32 reasonably-well-scrambled bits, based on the hash
//	of a given (signed) integer input parameter (position/index) and [optional] seed.  Kind of
//	like looking up a value in an infinitely large table of previously generated random numbers.
//
// The bit-noise constants and bit-shifts were evolved by a genetic algorithm using the
//	"BigCrush" statistical tests for fitness, and have so far produced excellent test results.
//
// I call this particular approach SquirrelNoise (version 4).
//
constexpr unsigned int Get1DNoiseUint(const int position_x, const unsigned int seed)
{
	constexpr unsigned int bit_noise1 = 0xd2a80a23;
	constexpr unsigned int bit_noise2 = 0xa884f197;
	constexpr unsigned int bit_noise3 = 0x1b56c4e9;

	unsigned int mangled_bits = static_cast<unsigned int>(position_x);
	mangled_bits *= bit_noise1;
	mangled_bits += seed;
	mangled_bits ^= (mangled_bits >> 7);
	mangled_bits += bit_noise2;
	mangled_bits ^= (mangled_bits >> 8);
	mangled_bits *= bit_noise3;
	mangled_bits ^= (mangled_bits >> 11);
	return mangled_bits;
}


//-----------------------------------------------------------------------------------------------
constexpr unsigned int Get2DNoiseUint(const int index_x, const int index_y, const unsigned int seed)
{
	constexpr int prime_number = 198491317; // Large prime number with non-boring bits
	return Get1DNoiseUint(index_x + (prime_number * index_y), seed);
}

//-----------------------------------------------------------------------------------------------
constexpr unsigned int Get3DNoiseUint(const int index_x, const int index_y, const int index_z, const unsigned int seed)
{
	constexpr int prime1 = 198491317; // Large prime number with non-boring bits
	constexpr int prime2 = 6542989; // Large prime number with distinct and non-boring bits
	return Get1DNoiseUint(index_x + (prime1 * index_y) + (prime2 * index_z), seed);
}

//-----------------------------------------------------------------------------------------------
constexpr unsigned int Get4DNoiseUint(const int index_x, const int index_y, const int index_z, const int index_t,
                                      const unsigned int seed)
{
	constexpr int prime1 = 198491317; // Large prime number with non-boring bits
	constexpr int prime2 = 6542989; // Large prime number with distinct and non-boring bits
	constexpr int prime3 = 357239; // Large prime number with distinct and non-boring bits
	return Get1DNoiseUint(index_x + (prime1 * index_y) + (prime2 * index_z) + (prime3 * index_t), seed);
}

//-----------------------------------------------------------------------------------------------
constexpr float Get1DNoiseZeroToOne(const int index, const unsigned int seed)
{
	constexpr double one_over_max_uint = (1.0 / static_cast<double>(0xFFFFFFFF));
	return static_cast<float>(one_over_max_uint * static_cast<double>(Get1DNoiseUint(index, seed)));
}

//-----------------------------------------------------------------------------------------------
constexpr float Get2DNoiseZeroToOne(const int index_x, const int index_y, const unsigned int seed)
{
	const double one_over_max_uint = (1.0 / static_cast<double>(0xFFFFFFFF));
	return static_cast<float>(one_over_max_uint * static_cast<double>(Get2DNoiseUint(index_x, index_y, seed)));
}

//-----------------------------------------------------------------------------------------------
constexpr float Get3DNoiseZeroToOne(const int index_x, const int index_y, const int index_z, const unsigned int seed)
{
	const double one_over_max_uint = (1.0 / static_cast<double>(0xFFFFFFFF));
	return static_cast<float>(one_over_max_uint * static_cast<double>(Get3DNoiseUint(index_x, index_y, index_z, seed)));
}

//-----------------------------------------------------------------------------------------------
constexpr float Get4DNoiseZeroToOne(const int index_x, const int index_y, const int index_z, const int index_t,
                                    const unsigned int seed)
{
	const double one_over_max_uint = (1.0 / static_cast<double>(0xFFFFFFFF));
	return static_cast<float>(one_over_max_uint * static_cast<double>(Get4DNoiseUint(
			index_x, index_y, index_z, index_t, seed))
	);
}


//-----------------------------------------------------------------------------------------------
constexpr float Get1DNoiseNegOneToOne(const int index, const unsigned int seed)
{
	const double one_over_max_uint = (1.0 / static_cast<double>(0x7FFFFFFF));
	return static_cast<float>(one_over_max_uint * static_cast<double>(static_cast<int>(Get1DNoiseUint(index, seed))));
}


//-----------------------------------------------------------------------------------------------
constexpr float Get2DNoiseNegOneToOne(const int index_x, const int index_y, const unsigned int seed)
{
	const double one_over_max_uint = (1.0 / static_cast<double>(0x7FFFFFFF));
	return static_cast<float>(one_over_max_uint * static_cast<double>(static_cast<int>(Get2DNoiseUint(
			index_x, index_y, seed)))
	);
}


//-----------------------------------------------------------------------------------------------
constexpr float Get3DNoiseNegOneToOne(const int index_x, const int index_y, const int index_z, const unsigned int seed)
{
	const double one_over_max_uint = (1.0 / static_cast<double>(0x7FFFFFFF));
	return static_cast<float>(one_over_max_uint * static_cast<double>(static_cast<int>(Get3DNoiseUint(
		index_x, index_y, index_z, seed))));
}


//-----------------------------------------------------------------------------------------------
constexpr float Get4DNoiseNegOneToOne(const int index_x, const int index_y, const int index_z, const int index_t,
                                      const unsigned int seed)
{
	const double one_over_max_uint = (1.0 / static_cast<double>(0x7FFFFFFF));
	return static_cast<float>(one_over_max_uint * static_cast<double>(static_cast<int>(Get4DNoiseUint(
		index_x, index_y, index_z, index_t, seed))));
}
