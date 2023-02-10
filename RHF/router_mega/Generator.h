#include <stdint.h>

//---------------------------------------------------------------------------
#ifndef GeneratorH
#define GeneratorH
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#pragma pack(push,1)
//---------------------------------------------------------------------------
double generateFunction1(const char *funcFileName, uint32_t *hash, int *pn,int nHash);
double generateFunction2(const char *funcFileName, uint32_t *hash, int *pn,int nHash);
double generateFunction3(const char *funcFileName, uint32_t *hash, int *pn,int nHash);

//---------------------------------------------------------------------------
#pragma pack(pop)
//---------------------------------------------------------------------------
#endif 
