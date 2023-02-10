//---------------------------------------------------------------------------
#ifndef PerfectRouterTester1H
#define PerfectRouterTester1H
//---------------------------------------------------------------------------
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include "../Array.h"
//---------------------------------------------------------------------------
#pragma pack(push,1)
//---------------------------------------------------------------------------
class TPerfectRouterTester1
{
public:
    TArray<int>  TdArray;
    TArray<int> TbucketArray;

    ~TPerfectRouterTester1()
    {
        Close();
    }

    void Close()
    {
        TdArray.Close();
        TbucketArray.Close();
    }

	bool Test(const char *funcFileName, uint32_t *hash, int *pn,int nHash);
};
#pragma pack(pop)
//---------------------------------------------------------------------------
#endif //Exclusive Include
