//---------------------------------------------------------------------------
#ifndef PerfectHashH
#define PerfectHashH
//---------------------------------------------------------------------------
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include "Type64.h"
#include "FastStrHash.h"
//---------------------------------------------------------------------------
#pragma pack(push,1)
//---------------------------------------------------------------------------
#define PHDataHdr "PerfectHash0.10"
//---------------------------------------------------------------------------
struct TPerfectHashData
{
    uint64_t r;
    uint64_t m;
	//In general d uses uint8_t is enough. For very difficult cases may change to uint16_t.
	uint8_t  d[sizeof(int64_t)/sizeof(uint8_t)]; //Causion: the number is dummy for syntax only, should be computed by builder!

	//The core hash function. For int data with length less than 64-bit, f(x)=x can be used for the first hash
	uint64_t Hash(uint64_t val)
	{
		return FastIntHash(val,d[val%r])%m;
	}

	//For variable length strings, it is hashed to 64-bit fixed length int first
	uint64_t HashStr(const char *str,int len=-1)
	{
		return Hash(FastStrHash(str,len));
	}
};
//---------------------------------------------------------------------------
#pragma pack(pop)
//---------------------------------------------------------------------------
#endif //Exclusive Include
