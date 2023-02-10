//---------------------------------------------------------------------------
#ifndef PerfectRouterTester3H
#define PerfectRouterTester3H
//---------------------------------------------------------------------------
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include "../Array.h"
//---------------------------------------------------------------------------
#pragma pack(push,1)
//---------------------------------------------------------------------------
class TPerfectRouterTester3
{
public:
    TArray<int>  TdArray;
    TArray<int> TbucketArray;

    ~TPerfectRouterTester3()
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
