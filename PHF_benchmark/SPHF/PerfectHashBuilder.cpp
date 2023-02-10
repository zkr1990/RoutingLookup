//---------------------------------------------------------------------------
#include <stdio.h>
#include "PerfectHashBuilder.h"
#include "PerfectHash.h"
#include "IntUtils.h"
//---------------------------------------------------------------------------
struct TSlotInfo
{
	uint64_t startPos;
	int64_t  nItem;
};
//---------------------------------------------------------------------------
int CmpSlot(const void *d1,const void *d2)
{
	// We assume nItem is much less than INT_MAX
	return (int)( ((TSlotInfo*)d2)->nItem - ((TSlotInfo*)d1)->nItem ); //Reverse order
}
//---------------------------------------------------------------------------
bool TPerfectHashBuilder::Build(uint64_t *hash,uint64_t nHash,uint64_t nBucket,int nPerSlot,uint64_t nSpace)
{
	if( (nHash==0) || (nPerSlot<=0) || ((uint64_t)nPerSlot>nHash) )
		return false;
	int64_t nSlot = RoundUpPrime(nHash*100/nPerSlot);

	if(nSlot==0)
		return false;

	if(nBucket==0)
		nBucket = RoundUpPrime(nHash);

    if(nBucket==0){
		fprintf(stderr,"nBucket: %d, too small to round up prime. \r\n", nBucket);
        return false;
	}

    if(nSpace==0)
        nSpace = nBucket;

	TArray<TSlotInfo> slots;
	slots.AllocFill(nSlot);
	slots.InitZero();
	
	for(uint64_t i=0;i<nHash;i++){
		slots[hash[i]%nSlot].nItem ++;
	}

	slots[0].startPos = slots[0].nItem-1;

	for(int64_t i=1;i<nSlot;i++){
		slots[i].startPos = slots[i].nItem+slots[i-1].startPos;
	}

	TArray<uint64_t> sortedHash;
	sortedHash.AllocFill(nHash);

	for(uint64_t i=0;i<nHash;i++)
	{
		uint64_t inx = hash[i]%nSlot;

		sortedHash[slots[inx].startPos] = hash[i];
		slots[inx].startPos --;
	}

	slots.QSort(CmpSlot);
	data.AllocFill(nSlot*sizeof(TPerfectHashData::d[0]));
	// data.AllocFill(16+nSlot*sizeof(TPerfectHashData::d[0]));
	data.InitZero();

	// TPerfectHashData *p = (TPerfectHashData *)data();
	// p->r = nSlot;
	// p->m = nBucket;

	TBitArray maskBits;
	maskBits.Alloc(nBucket);

	TArray<uint64_t> bitStack;
	bitStack.AllocFill(slots[0].nItem);

	for(int64_t i=0;i<nSlot;i++)
	{
		int      nItem    = (int) slots[i].nItem;

		if(nItem==0)
            continue;

		uint64_t startPos = slots[i].startPos+1;

		bool fail;

		//int64_t maxD = (1ull<<(sizeof(p->d[0])*8))-1;
		// fprintf(stderr,"maxD = %ld. \r\n", maxD);
		int64_t maxD = 255;
		for(int64_t d=0;d<maxD;d++)
		{
			fail = false;
			for(int j=0;j<nItem;j++)
			{
				uint64_t f = FastIntHash(sortedHash[startPos+j],d)%nSpace;
				if( (f>=nBucket) || (maskBits[f]) )
				{					
					while(j>0)
					{
						j--;
						maskBits.ClearBit(bitStack[j]);
					}
					fail = true;
					break;
				}

				maskBits.SetBit(f);
				bitStack[j] = f;
			}

			if(fail==false)
			{
				// p->d[sortedHash[startPos]%nSlot] = (uint16_t)d;
				data[sortedHash[startPos]%nSlot] = (uint16_t)d;
				break;
			}
		}

		if(fail)
			return false; // we can't find suitable d in [0,65535]
	}

	return true;
}

//---------------------------------------------------------------------------
