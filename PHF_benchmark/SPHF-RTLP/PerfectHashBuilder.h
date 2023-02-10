//---------------------------------------------------------------------------
#ifndef PerfectHashBuilderH
#define PerfectHashBuilderH
//---------------------------------------------------------------------------
#include <stdint.h>
#include <assert.h>
#include <string.h>
// #include "Hash/PerfectHash.h"
#include "Array.h"
// #include "SyncUtils.h"
//---------------------------------------------------------------------------
#pragma pack(push,1)
//---------------------------------------------------------------------------
class TBitArray
{
public:
    uint64_t *arr;
    int64_t   nBit;

    TBitArray()
    {
        arr  = NULL;
        nBit = 0;
    }

    ~TBitArray()
    {
        Close();
    }

    void Close()
    {
        if(arr)
        {
            delete arr;
            nBit = 0;
        }
    }

    void Alloc(int64_t _nBit)
    {
        Close();

        nBit = (_nBit+63)&~63ull;
        arr = new uint64_t[nBit>>6];
        ClearAll();
    }

    bool operator[](int64_t inx)
    {
        assert(inx>=0);
        assert(inx<nBit);
        return (arr[inx>>6]&(1ull<<(inx&63)))!=0;
    }

    void SetBit(int64_t inx)
    {
        assert(inx>=0);
        assert(inx<nBit);
        int64_t ip   = (inx>>6);
        int64_t mask = (1ull<<(inx&63));

        arr[ip] |= mask;
    }

    void ClearBit(int64_t inx)
    {
        assert(inx>=0);
        assert(inx<nBit);
        int64_t ip   = (inx>>6);
        int64_t mask = (1ull<<(inx&63));

        arr[ip] &= ~mask;
    }

    void ClearAll()
    {
        assert(arr);
        memset(arr,0,(nBit>>3));
    }
};//---------------------------------------------------------------------------
class TPerfectHashBuilder
{
public:
    TArray<uint16_t> data;

    ~TPerfectHashBuilder()
    {
        Close();
    }

    void Close()
    {
        data.Close();
    }

	//if nBucket==0, nBucket = nHash round up to prime.
	//To find MPH, set nBucket=nHash
	//nPerSlot= realNumItemPerSlot*100 (can less than 100, meaning avg less than 1 per slot)
	//nSpace is used to setup more bucket spaces to reduce the search difficulty on very difficult dataset.
	bool Build(uint64_t *hash,uint64_t nHash,int nBucket=65536,int nSlot=8192,int nSpace=0);
	bool Write(const char *fileName);

	void CopyIfBetter(TPerfectHashBuilder &o);
};
#pragma pack(pop)
//---------------------------------------------------------------------------
#endif //Exclusive Include
