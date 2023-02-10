//---------------------------------------------------------------------------
#include "Allocator.h"
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include "embdDbg.h"
//---------------------------------------------------------------------------
// If there is no suitable allocator, use the default C allocator.
//---------------------------------------------------------------------------
char *TAllocator::Malloc(size_t size,int align)
{
    logEmbdMsg("Prepare alloc %d-byte memory of align %d.\r\n",(int)size,align);
// #ifndef __LSeRTOS__
//     assert(align<=(int)sizeof(int));
// #endif // __LSeRTOS__
    return (char*)malloc(size);
}
//---------------------------------------------------------------------------
char *TAllocator::Realloc(void *mem,size_t size,int align,size_t oldSize)
{
    logEmbdMsg("Realloc %d-byte memory of align %d.\r\n",(int)size,align);
#ifndef __LSeRTOS__
    if(align>(int)sizeof(void *))
	{
		char *p = Malloc(size,align);
		if(p==NULL)
			return NULL;
		//This is WRONG. We should query the actual size of mem. But we havn't that interface yet.
		if(oldSize==0)
			oldSize = size;
		memcpy(p,mem,oldSize);
		Free(mem);
		return p;
	}
#endif // __LSeRTOS__

    return (char*)realloc(mem,size);
}
//---------------------------------------------------------------------------
void  TAllocator::Free(void *mem)
{
    free(mem);
}
//---------------------------------------------------------------------------
