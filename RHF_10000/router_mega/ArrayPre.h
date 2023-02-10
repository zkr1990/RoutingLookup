#pragma once
#ifndef ArrayPreH
#define ArrayPreH
//---------------------------------------------------------------------------
#include <assert.h>
#include <string.h>
#include <math.h>
#include "SortBase.h"
#include "Allocator.h"
// #include "Mem/FastMem.h"
#include "Type64.h"
//---------------------------------------------------------------------------
#ifndef NULL
#define NULL 0
#endif //NULL
//---------------------------------------------------------------------------
#pragma pack(push,1)
//---------------------------------------------------------------------------
class TArrayPubBase
{
protected:
    char *img;

public:
    int64_t len;
    int64_t Number;

protected:
    virtual void CallConstructor(void *obj) const = 0;
    virtual void CallDestructor(void *obj) const = 0;
    virtual bool IsTrivialElem() const = 0;
    virtual int  GetTypeSize() const = 0;

    void *New(int64_t expandUnit=64)
    {
        if(Number==len)
            ChCapacity(len+expandUnit);

        void *p = img+(size_t)Number*GetTypeSize();

        Number ++;

        return p;
    }

    void *Search(const void *key,TSortCmp cmp,int64_t n=-1) const;

    void *FastInsert(int64_t pos,int64_t newNum=1,bool keepOrder=false)
    {
        assert(pos>=0);
        assert(pos<=Number);
        assert(newNum>=1);

        if( (len-Number)<newNum )
            ChCapacity(Number+newNum);

        int typeSize = GetTypeSize();

        int64_t tail = Number;
        char   *mPos = img+(size_t)pos*typeSize;

        Number += newNum;

        if(keepOrder==false)
            memmove(img+(size_t)tail*typeSize,mPos,(size_t)newNum*typeSize);
        else
            memmove(mPos+(size_t)newNum*typeSize,mPos,(size_t)(tail-pos)*typeSize);

        return mPos;
    }

    void *FastDel(int64_t pos,int64_t delNum=1,bool keepOrder=false)
    {
        assert(pos>=0);
        assert(pos<=Number);
        assert(delNum>=1);

        int     typeSize = GetTypeSize();
        int64_t tail     = Number-delNum;
        char   *mPos     = img+(size_t)pos*typeSize;

        if(keepOrder==false)
            memcpy(mPos,img+(size_t)tail*typeSize,(size_t)delNum*typeSize);
        else
            memcpy(mPos,mPos+(size_t)delNum*typeSize,(size_t)(tail-pos)*typeSize);

        Number = tail;

        return mPos;
    }

    void FastBInsert(void *key,TSortCmp cmp,bool canRepeat=true);

public:
    TArrayPubBase()
    {
        img    = NULL;
        len    = 0;
        Number = 0;
    }

    virtual ~TArrayPubBase();

    void Close(bool noConstructor=false);

    void ClearDataNoFree()
	{
		img    = NULL;
		len    = 0;
		Number = 0;
	}

    void Alloc(int64_t maxNum,int align=32, bool noConstructor=false);
    void AllocFill(int64_t maxNum,int align=32, bool noConstructor=false)
    {
        Alloc(maxNum,align,noConstructor);
        Number = len;
    }
    void ChCapacity(int64_t newNum,int align=32);

	int64_t Size() const
    {
        return Number*GetTypeSize();
    }

	int64_t StorageSize() const
    {
        return len*GetTypeSize();
    }

    int64_t SpaceSize() const
    {
        return (len-Number)*GetTypeSize();
    }

	void *ObjAt(int64_t inx) const
	{
		assert(inx>=0);
		assert(inx<Number);

		return img+(size_t)(inx*GetTypeSize());
	}

	//Causion: if the underlying two arrays are not same type,
	//do not call Move() on them. Only compatiable typed arrays can be used.
	//For example: TArray<char> o.Move(TArray<unsigned char> src)
	void Move(TArrayPubBase &src)
	{
        Close();

		img        = src.img;
		len        = src.len;
		Number     = src.Number;

		src.img    = NULL;
		src.len    = 0;
		src.Number = 0;
	}

    void Grow(int64_t nGrow)
    {
        if((Number+nGrow)>len)
            ChCapacity(Number+nGrow);

        Number += nGrow;
    }
};
//---------------------------------------------------------------------------
template<class T>
int64_t ArrLargest(T *array,int64_t num)
{
    T *tmp;
    tmp = array;
    int64_t index = 0;

    for(int64_t i=1;i<num;i++)
    {
        if(array[i]>*tmp)
        {
            tmp   = array+i;
            index = i;
        }
    }

    return index;
}
//---------------------------------------------------------------------------
template<class T>
int64_t ArrSmallest(const T *array,int64_t num)
{
    T *tmp;
    tmp = array;
    int64_t index = 0;

    for(int64_t i=1;i<num;i++)
    {
        if(array[i]<*tmp)
        {
            tmp   = array+i;
            index = i;
        }
    }

    return index;
}
//---------------------------------------------------------------------------
template<class T>
void ArrMinMax(const T *array,int64_t num,int64_t &minIndex,int64_t &maxIndex)
{
    T *tMin = array;
    T *tMax = array;

    int64_t iMin=0, iMax=0;

    for(int64_t i=1;i<num;i++)
    {
        if(array[i]<*tMin)
        {
            *tMin = array+i;
            iMin  = i;
        }
        else if(array[i]>*tMax)
        {
            *tMax = array+i;
            iMax  = i;
        }
    }

    minIndex = iMin;
    maxIndex = iMax;
}
//---------------------------------------------------------------------------
template<class T>
void ArrTotal(const T *array,int64_t num,T &ret)
{
	ret = array[0];

	for(int64_t i=1;i<num;i++)
    	ret += array[i];
}
//---------------------------------------------------------------------------
template<class T>
inline void ArrMean(const T *array,int64_t num,T &ret)
{
    T tmp;

    ArrTotal(array,num,tmp);

    ret = tmp/num;
}
//---------------------------------------------------------------------------
//base e entropy
//---------------------------------------------------------------------------
template<class T>
double ArrEntropyE(const T *array,int64_t num)
{
    T sum;

    ArrTotal(array,num,sum);

    double e = 0;
    double m = 1.0/sum;

	for(int64_t i=0;i<num;i++)
    {
        double t = array[i]*m;

        if(t>1e-30)
        	e -= t*log(t);
    }

    return e;
}
//---------------------------------------------------------------------------
template<class T>
inline double ArrEntropy(const T *array,int64_t num)
{
    return ArrEntropyE(array,num)*1.4426950408889634073599246810019;          //log2(e)
}
//---------------------------------------------------------------------------
template<class T>
inline double ArrNormEntropy(const T *array,int64_t num)
{
    return ArrEntropyE(array,num)/log(num);
}
//---------------------------------------------------------------------------
#pragma pack(pop)
//---------------------------------------------------------------------------
#endif //ArrayH
