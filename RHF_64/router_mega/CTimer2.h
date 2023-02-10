#include <stdint.h>
#include "Builder/PerfectRouterBuilder2.h"

//---------------------------------------------------------------------------
#ifndef CTimer2H
#define CTimer2H
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#pragma pack(push,1)
//---------------------------------------------------------------------------、
class CTimer2
{
public:

    ~CTimer2()
    {
        Close();
    }

    void Close()
    {
    }

	double getCTime(uint64_t *hash, int *pn,int nHash, uint64_t &maxStorageCost);
    double getFileTime(const char * filename, uint64_t &maxStorageCost);
};


//---------------------------------------------------------------------------
#pragma pack(pop)
//---------------------------------------------------------------------------
#endif 
