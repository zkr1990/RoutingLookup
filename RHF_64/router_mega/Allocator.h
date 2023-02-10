//---------------------------------------------------------------------------
#ifndef AllocatorH
#define AllocatorH
//---------------------------------------------------------------------------
#include <stddef.h>
//---------------------------------------------------------------------------
#pragma pack(push,1)
//---------------------------------------------------------------------------
enum TAllocConsts
{
#ifdef __BORLANDC__
	acDefAlign = sizeof(int),
#else
	acDefAlign = 32,
#endif
};
//---------------------------------------------------------------------------
class TAllocator
{
public:
    char *Malloc(size_t size,int align=acDefAlign);
    char *Realloc(void *mem,size_t size,int align=acDefAlign,size_t oldSize=0);
    void  Free(void *mem);
};
//---------------------------------------------------------------------------
extern TAllocator defAllocator;
//---------------------------------------------------------------------------
//#if !defined(WIN32) && !defined(_WIN32)
//#include <linux/vmalloc.h>
//#else
void *vmalloc(size_t sz);
void  vfree(void *mem);
//#endif//WIN32
//---------------------------------------------------------------------------
void *vmallocReserve(size_t sz);
bool  vmallocCommit(void* ptr,size_t sz);
//---------------------------------------------------------------------------
#pragma pack(pop)
//---------------------------------------------------------------------------
#endif //Exclusive Include
