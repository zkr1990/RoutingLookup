#include <stdint.h>
#include "Builder/PerfectRouterBuilder1.h"

//---------------------------------------------------------------------------
#ifndef CTimer1H
#define CTimer1H
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#pragma pack(push,1)
//---------------------------------------------------------------------------、
class CTimer1
{
public:

    ~CTimer1()
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
