//---------------------------------------------------------------------------
#ifndef PerfectRouterBuilder1H
#define PerfectRouterBuilder1H
//---------------------------------------------------------------------------
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include "../Array.h"
//---------------------------------------------------------------------------
#pragma pack(push,1)
//------------------------ not used for now ------------------------------
// class TBitArray
// {
// public:
//     uint32_t *arr;
//     int64_t   nBit;

//     TBitArray()
//     {
//         arr  = NULL;
//         nBit = 0;
//     }

//     ~TBitArray()
//     {
//         Close();
//     }

//     void Close()
//     {
//         if(arr)
//         {
//             delete arr;
//             nBit = 0;
//         }
//     }

//     void Alloc(int64_t _nBit)
//     {
//         Close();

//         nBit = (_nBit+63)&~63ull;
//         arr = new uint32_t[nBit>>6]; // change uint64_t to uint32_t
//         ClearAll();
//     }

//     bool operator[](int64_t inx)
//     {
//         assert(inx>=0);
//         assert(inx<nBit);
//         return (arr[inx>>6]&(1ull<<(inx&63)))!=0;
//     }

//     void SetBit(int64_t inx)
//     {
//         assert(inx>=0);
//         assert(inx<nBit);
//         int64_t ip   = (inx>>6);
//         int64_t mask = (1ull<<(inx&63));

//         arr[ip] |= mask;
//     }

//     void ClearBit(int64_t inx)
//     {
//         assert(inx>=0);
//         assert(inx<nBit);
//         int64_t ip   = (inx>>6);
//         int64_t mask = (1ull<<(inx&63));

//         arr[ip] &= ~mask;
//     }

//     void ClearAll()
//     {
//         assert(arr);
//         memset(arr,0,(nBit>>3));
//     }
// };//---------------------------------------------------------------------------
class TPerfectRouterBuilder1
{
public:
    uint32_t r;
    uint32_t m;
    TArray<int>  dArray;
    TArray<int> bucketArray;
    // TArray<int> bucketStack;
    TArray<uint32_t> bucketItemStack;

    ~TPerfectRouterBuilder1()
    {
        Close();
    }

    void Close()
    {
        dArray.Close();
        bucketArray.Close();
        // bucketStack.Close();
        bucketItemStack.Close();
        r = (uint32_t) 0;
        m = (uint32_t) 0;
    }

	bool Build(int *pn,uint32_t *hash,int nHash,int nBucket=0,int nSlot=400);
	bool Write(const char *fileName);

	void CopyIfBetter(TPerfectRouterBuilder1 &o);
};
#pragma pack(pop)
//---------------------------------------------------------------------------
#endif //Exclusive Include
