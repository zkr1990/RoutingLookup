//---------------------------------------------------------------------------
#include <stdio.h>
#include "ArrayPre.h"
#include <malloc.h>
#include "embdDbg.h"
//---------------------------------------------------------------------------
TArrayPubBase::~TArrayPubBase()
{
}
//---------------------------------------------------------------------------
void TArrayPubBase::Alloc(int64_t maxNum,int align, bool noConstructor)
{
    
	logEmbdMsg("Prepare to alloc mem for array num=%d align=%d.\r\n",(int)maxNum,align);
    Close();

    logEmbdMsg("Array close ok.\r\n");

    int typeSize = GetTypeSize();

    logEmbdMsg("Get type size=%d ok.\r\n",typeSize);

    int64_t memSize = maxNum*typeSize;
#ifndef XrossCPP_In_64
    assert((memSize>>31)==0);//on 32-bit platforms, no memory alloc can larger than 2GB-1
#endif // XrossCPP_In_64

    logEmbdMsg("Ready to alloc %d-byte mem, defAllocator=%p.\r\n",(int)memSize,defAllocator);
    // fprintf(stderr,"memSize is: %lu\n", memSize);
    // fprintf(stderr,"align is: %d\n", align);
    
    img = defAllocator.Malloc((size_t)memSize,align);
    logEmbdMsg("Alloc returned img=%p.\r\n",img);

    assert(img);

    len = maxNum;

    logEmbdMsg("Ready to call constructor.\r\n");

	if( (noConstructor==false) && !IsTrivialElem() )
	{
        logEmbdMsg("Now call constructor.\r\n");

		for(int64_t i=0;i<maxNum;i++)
			CallConstructor(img+i*typeSize);
	}

    logEmbdMsg("Alloc mem for array ok.\r\n");
}
//---------------------------------------------------------------------------
void TArrayPubBase::Close(bool noConstructor)
{
    if(img)
    {
		if( (noConstructor==false) && !IsTrivialElem() )
		{
			int typeSize = GetTypeSize();

			for(int64_t i=0;i<len;i++)
				CallDestructor(img+i*typeSize);
		}

        defAllocator.Free(img);

        img    = NULL;
        len    = 0;
        Number = 0;
    }
}
//---------------------------------------------------------------------------
void TArrayPubBase::ChCapacity(int64_t newNum,int align)
{
    assert(newNum>=0);

    if(newNum==len)
        return;

    if(newNum==0)
    {
        Close();
        return;
    }

    int typeSize = GetTypeSize();

    if(newNum<len)
    {
        for(int64_t i=newNum;i<len;i++)
            CallDestructor(img+i*typeSize);
    }

    if(img==NULL)
        img = defAllocator.Malloc(newNum*typeSize,align);
    else
        img = defAllocator.Realloc(img,newNum*typeSize,align,len*typeSize);

    assert(img);

    if(newNum>len)
    {
        for(int64_t i=len;i<newNum;i++)
            CallConstructor(img+i*typeSize);
    }

    len = newNum;
}
//---------------------------------------------------------------------------
void *TArrayPubBase::Search(const void *key,TSortCmp cmp,int64_t n) const
{
    if(n<=0)
        n = Number;

    int typeSize = GetTypeSize();

    for(int64_t i=0;i<n;i++)
    {
        if(cmp(key,img+i*typeSize)==0)
            return img+i*typeSize;
    }

    return NULL;
}
//---------------------------------------------------------------------------
void TArrayPubBase::FastBInsert(void *key,TSortCmp cmp,bool canRepeat)
{
    int typeSize = GetTypeSize();

    if(Number==0)
    {
        memcpy(New(),key,typeSize);
        return;
    }

    int ret = cmp(&key,img+(Number-1)*typeSize);

    if(ret>=0)
    {
        if( (ret!=0) || canRepeat )
            memcpy(New(),key,typeSize);

        return;
    }

    int64_t low  = 0;
    int64_t high = Number;

    while(low<high)
    {
        int64_t mid = ((low+high)>>1);

        ret = cmp(&key,img+mid*typeSize);

        if(ret==0)
        {
            if(canRepeat)
                memcpy(FastInsert(mid,1,true),key,typeSize);

            return;
        }

        if(ret<0)
            high = mid;
        else
            low  = mid+1;
    }

    if(cmp(&key,img+low*typeSize)>0)
        low ++;

    memcpy(FastInsert(low,1,true),key,typeSize);
}
//---------------------------------------------------------------------------
