//---------------------------------------------------------------------------
#include <stdio.h>
#include <iostream>
#include <bitset>
#include "PerfectRouterBuilder3.h"
#include "../IntUtils.h"
//---------------------------------------------------------------------------
struct TSlotInfo
{
	uint32_t startPos;
	int32_t  nItem;
	int index;
};
//---------------------------------------------------------------------------
int CmpSlot3(const void *d1,const void *d2)
{
	// We assume nItem is much less than INT_MAX
	return (int)( ((TSlotInfo*)d2)->nItem - ((TSlotInfo*)d1)->nItem ); //Reverse order
}
//---------------------------------------------------------------------------
bool TPerfectRouterBuilder3::Build(int *pn, uint64_t *hash,int nHash,int nBucket,int nSlot)
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
	int shiftBucket = 64 - bucketWidth;

    int nSlotWidth = (int)(log(nSlot)/log(2));
    int shiftSlot = 64 - nSlotWidth;

	// update information for each slot
	for(int i=0;i<nHash;i++){
		int idx = (hash[i] << shiftSlot) >> shiftSlot; // int idx = hash[i]%nSlot;
		slots[idx].nItem ++;
		slots[idx].index = idx;
	}

	// begin key - item mapping
	slots[0].startPos = slots[0].nItem-1;

	for(int i=1;i<nSlot;i++){
		slots[i].startPos = slots[i].nItem+slots[i-1].startPos;
	}
		
	TArray<uint64_t> sortedHash;
	sortedHash.AllocFill(nHash);
	TArray<int> sortedPn;
	sortedPn.AllocFill(nHash);

	for(int i=0;i<nHash;i++)
	{
		int idx = (hash[i] << shiftSlot) >> shiftSlot; // uint32_t idx = hash[i]%nSlot;		

		sortedHash[slots[idx].startPos] = hash[i];
		sortedPn[slots[idx].startPos] = pn[i];
		slots[idx].startPos --;
	}
	// finish key - item mapping

	// sort slot using number of items, in descending order
	slots.QSort(CmpSlot3);
	
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

		// int maxD = (int)(log(nSlot)/log(2));
		// fprintf(stderr,"maxD: %d\r\n", maxD);
		// int maxD = 255;
		int maxD = 255;
		for(int d=1;d<maxD;d++)
		{
			fail = 0;
			// re-initialize the bucketItemStack
			bucketItemStack.InitZero();

			for(int j=0;j<nItem;j++)
			{
				uint64_t x_key = sortedHash[startPos+j] >> 10;
				// Single multiplication approach (not efficient for FPGA DSP block)
				// int f = ( ((x_key * (uint64_t)d) >> 16) << shiftBucket >> shiftBucket);
				
				// x_key has values in lower 56 bits, x_key[63:56] = 00000000
				// dsp48E max multiplier input width: 25 bits
				// for lower 25 bits (64 - 25 = 39): x_key << 39 >> 39
				// for higher (56 - 25 = 31) bits: x_key >> 25
				uint64_t add1A = (x_key >> 50);
				uint64_t add1B = (x_key >> 36);
				uint64_t add2C = (x_key >> 23);
				uint64_t add2D = (x_key >> 10);

				int f = ((add1A + add1B * (uint64_t)d) ^ (add2C + add2D * (uint64_t)d)) << shiftBucket >> shiftBucket;
				// int f = ( (((x_key << 39 >> 39) * (uint64_t)d + (x_key >> 25)) >> 13) << shiftBucket >> shiftBucket);
				
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
