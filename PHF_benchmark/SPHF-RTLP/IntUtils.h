//---------------------------------------------------------------------------
#ifndef IntUtilsH
#define IntUtilsH
//---------------------------------------------------------------------------
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include "Type64.h"
//---------------------------------------------------------------------------
#pragma pack(push,1)
//---------------------------------------------------------------------------
uint64_t MulHigh(uint64_t val1,uint64_t val2);
void Mul64(uint64_t val1,uint64_t val2,uint64_t &high,uint64_t &low);
//---------------------------------------------------------------------------
// Following functions can be optimized by asm
// mul1*mul2 -> 64-bit result%d
inline uint32_t /*__fastcall*/ MulMod(uint32_t m1,uint32_t m2,uint32_t d)
{
    return (uint32_t)(((uint64_t)m1*m2)%d);
}
// p1*p2 -> 64-bit result+c
inline uint64_t /*__fastcall*/ MulAdd(uint32_t m1,uint32_t m2,uint32_t c)
{
    return ((uint64_t)m1*m2)+c;
}
// m1*m2 -> 64-bit result
inline uint64_t /*__fastcall*/ Mul(uint32_t m1,uint32_t m2)
{
    return ((uint64_t)m1*m2);
}
//---------------------------------------------------------------------------
uint64_t RoundUpPow2(uint64_t val);
uint64_t RoundUpPrime(uint64_t val);
/*---------------------------------------------------------------------------
Rabin-Miller primality test for up to max uint64_t.
Causion: we think 0, 1, 2 are not prime.
Causion: only work on x64 platform.
---------------------------------------------------------------------------*/
bool RM_PrimeTest(uint64_t n);
//---------------------------------------------------------------------------
//High efficient mul and div funcs for x64 platform only
//#ifdef XrossCPP_In_X64
#ifdef __cplusplus
extern "C"
{
#endif//__cplusplus
//---------------------------------------------------------------------------
/* (m1*m2) % modV */
uint64_t Mul64Mod(uint64_t modV,uint64_t m1,uint64_t m2);
/* (m1*m2) / divV */
uint64_t Mul64Div(uint64_t divV,uint64_t m1,uint64_t m2);
/* (m1*m2) /% divV, return both */
TX128Data Mul64DivFull(uint64_t divV,uint64_t m1,uint64_t m2);
//---------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif//__cplusplus
//#endif //XrossCPP_In_X64
//---------------------------------------------------------------------------
#pragma pack(pop)
//---------------------------------------------------------------------------
#endif //Exclusive Include
