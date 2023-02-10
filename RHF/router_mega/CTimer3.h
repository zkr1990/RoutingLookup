#include <stdint.h>
#include "Builder/PerfectRouterBuilder3.h"

//---------------------------------------------------------------------------
#ifndef CTimer3H
#define CTimer3H
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#pragma pack(push,1)
//---------------------------------------------------------------------------、
class CTimer3
{
public:

    ~CTimer3()
    {
        Close();
    }

    void Close()
    {
    }

	double getCTime(uint32_t *hash, int *pn, int nHash, uint64_t &maxStorageCost);
    double getFileTime(const char * filename, uint64_t &maxStorageCost);
};


//---------------------------------------------------------------------------
#pragma pack(pop)
//---------------------------------------------------------------------------
#endif 
