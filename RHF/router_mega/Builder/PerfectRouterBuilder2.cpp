//---------------------------------------------------------------------------
#include <stdio.h>
#include <iostream>
#include <bitset>
#include "PerfectRouterBuilder2.h"
#include "../IntUtils.h"
#include "../City64.h"
//---------------------------------------------------------------------------
struct TSlotInfo
{
	uint32_t startPos;
	int32_t  nItem;
	int index;
};
//---------------------------------------------------------------------------
int CmpSlot2(const void *d1,const void *d2)
{
	// We assume nItem is much less than INT_MAX
	return (int)( ((TSlotInfo*)d2)->nItem - ((TSlotInfo*)d1)->nItem ); //Reverse order
}
//---------------------------------------------------------------------------
bool TPerfectRouterBuilder2::Build(int *pn, uint32_t *hash,int nHash,int nBucket,int nSlot)
// bool TPerfectRouterBuilder2::Build(int *pn, uint32_t *hash,int nHash,int nBucket,int nSlot, int sl0, int sl1, int s0, int s1)
{
	if(nSlot==0)
		return false;

    if(nBucket==0){
		fprintf(stderr,"nBucket: %d, too small to round up prime. \r\n", nBucket);
        throw "exception, nBucket is zero!";
		return false;
	}
	TArray<TSlotInfo> slots;
	slots.AllocFill(nSlot);
	slots.InitZero();
    // bit-shift represents the mod operation
	int bucketWidth = (int) (log(nBucket)/log(2));
	int shiftBucket = 32 - bucketWidth;

    int nSlotWidth = (int)(log(nSlot)/log(2));
    int shiftSlot = 32 - nSlotWidth;

	// update information for each slot
	for(int i=0;i<nHash;i++){
		// int idx = ( ((hash[i] >> sl0) ^ (hash[i] << sl1) ^ hash[i]) << shiftSlot >> shiftSlot);
		int idx = (hash[i] << shiftSlot) >> shiftSlot; // int idx = hash[i]%nSlot;
		slots[idx].nItem ++;
		slots[idx].index = idx;
	}

	// begin key - item mapping
	slots[0].startPos = slots[0].nItem-1;

	for(int i=1;i<nSlot;i++){
		slots[i].startPos = slots[i].nItem+slots[i-1].startPos;
	}
		
	TArray<uint32_t> sortedHash;
	sortedHash.AllocFill(nHash);
	TArray<int> sortedPn;
	sortedPn.AllocFill(nHash);

	for(int i=0;i<nHash;i++)
	{
		// int idx = ( ((hash[i] >> sl0) ^ (hash[i] << sl1) ^ hash[i]) << shiftSlot >> shiftSlot);
		int idx = (hash[i] << shiftSlot) >> shiftSlot; // uint32_t idx = hash[i]%nSlot;		

		sortedHash[slots[idx].startPos] = hash[i];
		sortedPn[slots[idx].startPos] = pn[i];
		slots[idx].startPos --;
	}
	// finish key - item mapping

	// sort slot using number of items, in descending order
	slots.QSort(CmpSlot2);
	
	// initialize dArray, bucketArray and bucketItemStack
	dArray.AllocFill(nSlot * 4); // 16bit x nSlot
	dArray.InitZero();

	bucketArray.AllocFill(nBucket * 4); // 32bit x number of buckets
	bucketArray.InitZero();

	bucketItemStack.AllocFill(slots[0].nItem * 4); // 32bit x item number of slots[0]
	bucketItemStack.InitZero();
	
	/* begin building */
	for(int i=0;i<nSlot;i++)
	{
		int nItem = (int) slots[i].nItem;
		int index = slots[i].index;

		if(nItem==0)
            continue;

		uint32_t startPos = slots[i].startPos+1;

		int fail = 1;

		int maxD = 32767;
		// int maxD = 65535;
		for(int d=1;d<maxD;d++)
		{
			fail = 0;
			// re-initialize the bucketItemStack
			bucketItemStack.InitZero();

			for(int j=0;j<nItem;j++)
			{
				// uint32_t x_key = sortedHash[startPos+j];
				uint32_t x_key = sortedHash[startPos+j] >> 10;
				// int f = ( ((x_key + (uint32_t)d) >> 8) << shiftBucket >> shiftBucket);
				// int f = FastIntHash(x_key, (uint32_t)d) << shiftBucket >> shiftBucket;

				uint32_t k2 = (uint32_t) 0x40000007;
				uint32_t kMul = (uint32_t) 0x40000003;
				uint32_t a = ((x_key - k2) ^ (uint32_t)d) * kMul;
				a ^= (a >> 13);
				uint32_t b = ((uint32_t)d ^ a) * kMul;
				b ^= (b >> 13);
				b *= kMul;
				int f = b << shiftBucket >> shiftBucket;

				int pnGT = sortedPn[startPos+j];

				if (bucketArray[f] > 0)
				{
					if (bucketArray[f] == pnGT)
					{
						bucketItemStack[j] = nBucket; // mark as bad value
						continue;
					} else
					{
						int j1 = j;
						while(j1>0)
						{
							j1--;
							if (bucketItemStack[j1] < nBucket)
							{
								bucketArray[bucketItemStack[j1]] = 0; // not bad value, delete
							}
						}
						fail = 1;
						break;
					}
				} else
				{
					// if (sortedHash[startPos+j] == (uint32_t)179551886)
					// {
					// 	// int idx_d = sortedHash[startPos+j]%nSlot;
					// 	int idx_d = ((sortedHash[startPos+j] << shiftSlot) >> shiftSlot);
					// }
					bucketArray[f] = pnGT;
                    bucketItemStack[j] = f;
				}
			}
			if(fail==0) // current d is successfully found
			{
				dArray[index] = d; // int idx_d = sortedHash[]%nSlot;
				break;
			}
		}
		if(fail==1) // we can't find suitable d in [0,maxD-1], building failure
		{
			sortedHash.Close();
			sortedPn.Close();
			slots.Close();
			return false; 
		}
	}
	/* end building */

	// if success, update the r and m for nSlot and nBucket
	// bucketArray, dArray already updated
	r = nSlot;
	m = nBucket;
	sortedHash.Close();
	sortedPn.Close();
	slots.Close();
	return true;
}

//---------------------------------------------------------------------------
