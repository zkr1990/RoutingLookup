//---------------------------------------------------------------------------
#include "IntUtils.h"
#include <math.h>
// #include "Sys/CpuUtils.h"
//---------------------------------------------------------------------------
uint64_t MulHigh(uint64_t val1,uint64_t val2)
{
#if defined(XrossCPP_In_64)
    return ((__int128_t)val1 * val2) >> 64;
#else
    uint32_t v1L = (uint32_t)(val1&0xfffffffful);
    uint32_t v1H = (uint32_t)((val1>>32)&0xfffffffful);
    uint32_t v2L = (uint32_t)(val2&0xfffffffful);
    uint32_t v2H = (uint32_t)((val2>>32)&0xfffffffful);

    uint64_t mLL = (uint64_t)v1L*v2L;

    uint64_t mHL = (uint64_t)v1H*v2L;
    uint64_t mLH = (uint64_t)v1L*v2H;

    uint64_t m1  = mHL+mLH;
    uint64_t m2  = m1 +(mLL>>32);

    uint64_t c =  ( (mHL&mLH) | ((mHL^mLH)&~m2) ) & (1ull<<63);

    return (m2>>32) + (uint64_t)v1H*v2H + (c>>31);
#endif
}
//---------------------------------------------------------------------------
void Mul64(uint64_t val1,uint64_t val2,uint64_t &high,uint64_t &low)
{
#if defined(XrossCPP_In_64)
    __int128_t tmp = (__int128_t)val1 * val2;
    high = (uint64_t)(tmp>>64);
    low  = (uint64_t) tmp;
#else
    uint32_t v1L = (uint32_t)(val1&0xfffffffful);
    uint32_t v1H = (uint32_t)((val1>>32)&0xfffffffful);
    uint32_t v2L = (uint32_t)(val2&0xfffffffful);
    uint32_t v2H = (uint32_t)((val2>>32)&0xfffffffful);

    uint64_t mLL = (uint64_t)v1L*v2L;

    uint64_t mHL = (uint64_t)v1H*v2L;
    uint64_t mLH = (uint64_t)v1L*v2H;

    uint64_t m1  = mHL+mLH;
    uint64_t m2  = m1 +(mLL>>32);

    uint64_t c =  ( (mHL&mLH) | ((mHL^mLH)&~m2) ) & (1ull<<63);

    high = (m2>>32) + (uint64_t)v1H*v2H + (c>>31);
    low  = mLL+(m1<<32);
#endif
}
//---------------------------------------------------------------------------
uint64_t RoundUpPow2(uint64_t val)
{
	val--;
	val |= val >> 1;
	val |= val >> 2;
	val |= val >> 4;
	val |= val >> 8;
	val |= val >> 16;
	val |= val >> 32;
	return val+1;
}
//---------------------------------------------------------------------------
// We havn't implement them yet
//---------------------------------------------------------------------------
/*
#ifndef XrossCPP_In_64
uint64_t Mul64Mod(uint64_t modV,uint64_t m1,uint64_t m2)
{
    assert(0);
}
uint64_t Mul64Div(uint64_t divV,uint64_t m1,uint64_t m2)
{
    assert(0);
}
TX128Data Mul64DivFull(uint64_t divV,uint64_t m1,uint64_t m2)
{
    assert(0);
}
#endif // XrossCPP_In_64
*/
// //---------------------------------------------------------------------------
// static bool RM_witness(uint64_t n, uint64_t a, uint64_t s, unsigned tMinus1)
// {
// 	uint64_t v = 1;

// 	while (s > 0)
// 	{
// 		if ( (s&1) == 1 )
// 			v = Mul64Mod(n,v,a);

// 		a = Mul64Mod(n,a,a);
// 		s >>= 1;
// 	}

// 	if (1 == v)
// 		return true;

// 	uint64_t nM1 = n-1;

// 	for (unsigned i = 0; v != nM1; i++)
// 	{
// 		if (i == tMinus1)
// 			return false;

// 		v = Mul64Mod(n,v,v);
// 	}

// 	return true;
// }
//---------------------------------------------------------------------------
bool RM_PrimeTest(uint64_t p)  
{  
     uint64_t x = (uint64_t)sqrt(p);  
     uint64_t i = 3;  
     if(p==2)return true;  
     if(p%2 == 0)return false;  
     while(i<x+1){  
                if(p%i == 0)return false;  
                i+=2;  
                }  
     return true;  
}
// bool RM_PrimeTest(uint64_t n)
// {
// 	/* NB: witness a must be 1 <= a < n, so we check some small numbers first */
// 	if (n < 3 || (n&1) == 0) // we think 0, 1 & 2 are not prime
// 		return false;

// 	if(n<=19) //for all odd less than 19, only 9 and 15 not prime
// 	{
// 		return (n!=9) && (n!=15);
// 	}

// 	/* derive 2^t * s = n - 1 where s is odd */
// 	uint64_t s = n-1;
// 	unsigned t = GetTailZero(s);
// 	s >>= t;

// 	t--; // we use t-1 all the time

// 	/*
// 	 * Witness 2 is deterministic for all n < 2047. Witnesses 2, 7, 61
// 	 * are deterministic for all n < 4,759,123,141.
// 	 */

// 	if (RM_witness(n, 2, s, t)==false)
// 		return false;

// 	if (n < 2027)
// 		return true;

// 	if (RM_witness(n, 7, s, t)==false)
// 		return false;

// 	if(n<4759123141ull)
// 		return RM_witness(n, 61, s, t);

// 	static const uint8_t witness64[] = { 3, 5, 11, 13, 17, 19, 23, 29, 31, 37 }; // 2 & 7 are tested already

// 	for (unsigned i = 0; i < (sizeof(witness64)/sizeof(witness64[0])); i++)
// 	{
// 		if (!RM_witness(n, witness64[i], s, t))
// 			return false;
// 	}

// 	return true;
// }
//---------------------------------------------------------------------------
uint64_t RoundUpPrime(uint64_t val)
{
    val |= 1;

    while( (RM_PrimeTest(val)==false) && (val>1) )
        val += 2;

    return (val<=1?0:val);
}
//---------------------------------------------------------------------------
