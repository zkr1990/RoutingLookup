#pragma once
#ifndef ArrayH
#define ArrayH
//---------------------------------------------------------------------------
#include <stdlib.h>
#include "ArrayPre.h"
// #include "Mem/FastMem.h"
#include <new>
#if defined(__BORLANDC__) || defined(__LSeRTOS__)
#define __is_trivial(x) false
#endif// __BORLANDC__
//#include <type_traits>
//---------------------------------------------------------------------------
#pragma pack(push,1)
//---------------------------------------------------------------------------
#define DefCommArrMath(func,retType)          \
    retType func(int64_t num=0)  const        \
    {                                         \
        if(num<=0)                            \
            num = Number;                     \
                                              \
        return Arr##func(&ObjConst(0),num);   \
    }
//---------------------------------------------------------------------------
// Causion: only one-time allocated array's alignment is preserved!
// Any increasement with New() will break the alignment set by Alloc.
//---------------------------------------------------------------------------
template<class T>
class TArraySafe:public TArrayPubBase
{
protected:
    virtual void CallConstructor(void *obj) const
    {
        new (obj) T;
    }

    virtual void CallDestructor(void *obj) const
    {
        ((T *)obj)->~T();
    }

    virtual bool IsTrivialElem() const
	{
		return __is_trivial(T);
	}

    virtual int  GetTypeSize() const
    {
        return sizeof(T);
    }

public:
	TArraySafe()
    {
    }

	TArraySafe(int64_t num)
    {
        Alloc(num);
    }

	virtual ~TArraySafe()
	{
		Close();
	}

	void Transfer(TArraySafe<T> &o)
	{
		Close();
		img    = o.img;
		len    = o.len;
		Number = o.Number;

        o.ClearDataNoFree();
	}

    T &New(int64_t expandUnit=64)
    {
        return *(T*)TArrayPubBase::New(expandUnit);
    }

	T &Obj(int64_t i)
    {
        assert(img);
        assert(i>=0);
        assert(i<Number);

        return *(T *)(img+i*sizeof(T));
    }

	const T &ObjConst(int64_t i) const
    {
        assert(img);
        assert(i>=0);
        assert(i<Number);

        return *(T *)(img+i*sizeof(T));
    }

    T &operator[](int64_t i)
    {
        return Obj(i);
    }

    const T &operator[](int64_t i) const
    {
        return ObjConst(i);
    }

    T *operator()()
    {
        return (T*)img;
    }

    const T *operator()() const
    {
        return (const T*)img;
    }

	T *Space(int64_t minSlot)
	{
		if(img==NULL)
		{
			Alloc(minSlot);
		}
		else
		{
			int64_t left = len-Number;
			if(left<minSlot)
			{
				ChCapacity(Number+minSlot);
			}
		}

		return ((T*)img)+Number;
	}

	TArraySafe<T> &operator=(TArraySafe<T> &o)
    {
        Alloc(o.Number);

        for(int64_t i=0;i<o.Number;i++)
            New() = o[i];

        return *this;
    }

    void operator += (T &o)
    {
        New() = o;
    }

    //slow but type-safe insert at pos.
    T *SlowInsert(int64_t pos,int64_t newNum=1,bool keepOrder=false)
    {
        assert(pos>=0);
        assert(pos<=Number);
        assert(newNum>=1);

        if( (len-Number)<newNum )
            ChCapacity(Number+newNum);

        int tail = Number;

        Number += newNum;

        if(keepOrder==false)
        {
            for(int64_t i=0;i<newNum;i++)
                Obj(tail+i) = Obj(pos+i);
        }
        else
        {
            for(int64_t i=tail-1;i>=pos;i--)
                Obj(i+newNum) = Obj(i);
        }

        return &Obj(pos);
    }

    T *SlowDel(int64_t pos,int64_t delNum=1,bool keepOrder=false)
    {
        assert(pos>=0);
        assert(pos<=Number);
        assert(delNum>=1);

        int tail = Number-delNum;

        if(keepOrder==false)
        {
            for(int64_t i=0;i<delNum;i++)
                Obj(pos+i) = Obj(tail+i);
        }
        else
        {
            for(int64_t i=pos;i<tail;i++)
                Obj(i) = Obj(i+delNum);
        }

        Number = tail;

        return &Obj(pos);
    }

	T *BSearch(const void *key,TSortCmp cmp,int n=-1) const
	{
		if(n<=0)
			n = Number;

		return (T*)(bsearch(key,img,n,sizeof(T),cmp));
	}

    T *Search(const void *key,TSortCmp cmp,int64_t n=-1) const
    {
        return (T*)(TArrayPubBase::Search(key,cmp,n));
    }

	void QSort(TSortCmp cmp,int64_t n=-1)
	{
		if(n<=0)
			n = Number;

        qsort(img,n,sizeof(T),cmp);
	}

    DefCommArrMath(Largest,int64_t)
    DefCommArrMath(Smallest,int64_t)
    DefCommArrMath(EntropyE,double)
    DefCommArrMath(Entropy,double)
    DefCommArrMath(NormEntropy,double)

    void MinMax(int64_t &minIndex,int64_t &maxIndex,int64_t num=0) const
    {
        if(num<=0)
            num = Number;

        ArrMinMax(&Obj(0),num,minIndex,maxIndex);
    }

    void Total(T &ret,int64_t num=0) const
    {
        if(num<=0)
            num = Number;

        ArrTotal(&Obj(0),num,ret);
    }

    void Mean(T &ret,int64_t num=0) const
    {
        if(num<=0)
            num = Number;

        ArrMean(&Obj(0),num,ret);
    }
};
//---------------------------------------------------------------------------
// Causion: types with internal resources or virtual functions  cannot use
// this class! Use TArraySafe insdead.
//---------------------------------------------------------------------------
template<class T>
class TArray:public TArraySafe<T>
{
public:
	TArray()
	{
	}

	TArray(int64_t num)
    :TArraySafe<T>(num)
    {
    }

	void InitZero()
    {
        memset(this->img,0,sizeof(T)*this->len);
    }

    void FastInit(T val)
    {
        MemSetType(this->img,val,sizeof(T)*this->len);
    }

	void FastCopy(TArray<T> &o,int64_t l=-1)
	{
		if( (l<0) || (l>o.Number) )
			l = o.Number;

        FastCopy(o(),l);
	}

	void FastCopy(T *o,int64_t l)
	{
		this->Alloc(l);
		memcpy(this->img,o,sizeof(T)*l);
        TArraySafe<T>::Number = l;
	}

    T *FastInsert(int64_t pos,int64_t newNum=1,bool keepOrder=false)
    {
        return (T*)TArrayPubBase::FastInsert(pos,newNum,keepOrder);
    }

    T *FastDel(int64_t pos,int64_t delNum=1,bool keepOrder=false)
    {
        return (T*)TArrayPubBase::FastDel(pos,delNum,keepOrder);
    }

    void FastBInsert(T &obj,TSortCmp cmp,bool canRepeat=false)
    {
        TArrayPubBase::FastBInsert(&obj,cmp,canRepeat);
    }
};
//---------------------------------------------------------------------------
typedef TArray<uint8_t> TGrayArray;
//---------------------------------------------------------------------------
#pragma pack(pop)
//---------------------------------------------------------------------------
#endif //ArrayH

