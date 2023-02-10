#pragma once
#ifndef Type64H
#define Type64H
//---------------------------------------------------------------------------
#include <stdint.h>
//---------------------------------------------------------------------------
#pragma pack(push,1)
//---------------------------------------------------------------------------
#undef XrossCPP_In_64
#undef XrossCPP_In_32
#undef XrossCPP_In_IA64
#undef XrossCPP_In_PC
#undef XrossCPP_In_X64
#undef XrossCPP_In_X86
#undef XrossCPP_In_ARM
#undef XrossCPP_In_ARM_thumb
//---------------------------------------------------------------------------
#ifdef __GNUC__
#	if defined (__amd64__) || defined (__amd64) || defined (__x86_64__) || defined (__x86_64)
#		define XrossCPP_In_X64
#		define XrossCPP_In_64
#       define XrossCPP_In_PC
#	elif defined (_IA64) || defined (__ia64__) || defined (__IA64__)
#		define XrossCPP_In_IA64
#		define XrossCPP_In_64
#	elif defined (__i386__) || defined (__i486__) || defined (__i586__) || defined (__i686__)
#		define XrossCPP_In_X86
#		define XrossCPP_In_32
#       define XrossCPP_In_PC
#	elif defined (__arm__)
#		if defined (__thumb__)
#			define XrossCPP_In_ARM_thumb
#		endif
#		define XrossCPP_In_ARM
#		define XrossCPP_In_32
#	endif
#elif defined (__BORLANDC__)
#	define XrossCPP_In_X86
#	define XrossCPP_In_32
#   define XrossCPP_In_PC
#endif
//---------------------------------------------------------------------------
#if !defined(WIN32) && !defined(_WIN32)
#define __fastcall
#endif
//---------------------------------------------------------------------------
#ifdef XrossCPP_In_64
	typedef int64_t sint_t;
	typedef uint64_t uint_t;
#elif !defined(__LSeRTOS__)
	typedef int32_t sint_t;
	typedef uint32_t uint_t;
#endif
//---------------------------------------------------------------------------
union TX64Data
{
    int64_t  i64;
    uint64_t u64;

    int32_t i32[2];
    int16_t i16[4];
    int8_t  i8[8];

    uint32_t u32[2];
    uint16_t u16[4];
    uint8_t  u8[8];

    double df;
    float  f[2];
};
//---------------------------------------------------------------------------
#define x64Low32(x) ((TX64Data*)&x)->u32[0]
#define x64High32(x) ((TX64Data*)&x)->u32[1]
//---------------------------------------------------------------------------
union TX128Data
{
	TX64Data x64[2];
    int64_t  i64[2];
    uint64_t u64[2];

    int32_t i32[4];
    int16_t i16[8];
    int8_t  i8[16];

    uint32_t u32[4];
    uint16_t u16[8];
    uint8_t  u8[16];

    double df[2];
    float  f[4];

	TX128Data()
	{
	}

	TX128Data(uint64_t vL,uint64_t vH)
	{
		u64[0] = vL;
		u64[1] = vH;
	}
};
//---------------------------------------------------------------------------
typedef TX128Data int128_t;
//---------------------------------------------------------------------------
#define x128Low64(x)  ((TX128Data*)&x)->u64[0]
#define x128High64(x) ((TX128Data*)&x)->u64[1]
//---------------------------------------------------------------------------
#pragma pack(pop)
//---------------------------------------------------------------------------
#endif // Type64H
