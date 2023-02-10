#include <stdio.h>
#include <cstring>
#include <fstream>
#include <cmath>
#include <sys/time.h>
#include "CTimer1.h"
#include <math.h>

//------------------------------------------------------------------------------
using namespace std;
struct rmInfo
{
	int nSlot;
	int nBucket;
	int storageCost;
};
int CmpRM1(const void *d1,const void *d2)
{
	// We assume nItem is much less than INT_MAX
	return (int)( ((rmInfo*)d1)->storageCost - ((rmInfo*)d2)->storageCost ); //Reverse order
}
double CTimer1::getCTime(uint32_t *hash, int *pn,int nHash, uint64_t &maxStorageCost)
{
    ///////////////////////////////////
    // we fix the nBucket and nSlot to compare construction time
    // nSlot and nBucket "must be" powers of 2.
    // start with  minimum number slots
    int nSlotStart = 256;
    int nSlotEnd = 2048;
    int nSlotStep = log(int(nSlotEnd/nSlotStart))/log(2) + 1;
    // start with minimum number of buckets
    int nBucketStart = 4096;
    int nBucketEnd = 32768; // this number must be larger than 4
    int nBucketStep = log(int(nBucketEnd/nBucketStart))/log(2) + 1;

    // sort rmArray
	TArray<rmInfo> rmArray;
	rmArray.AllocFill(nSlotStep*nBucketStep);
	rmArray.InitZero();

    int ind = 0;
    int r = nSlotStart;
    while(r <= nSlotEnd)
    {
        int m = nBucketStart;
        while(m <= nBucketEnd)
        {
            rmArray[ind].nSlot = r;
            rmArray[ind].nBucket = m;
            rmArray[ind].storageCost = r*11 + m*2;
            m = m * 2;
            ind++;
        }
        r = r * 2;
    }
    // for (int i=0; i < ind; i++)
    // {
    //     fprintf(stderr,"i: %d, rmArray: %d %d %d \r\n", i, \
    //     rmArray[i].nSlot, rmArray[i].nBucket, rmArray[i].storageCost);
    // }
    // fprintf(stderr,"-----\n");
    rmArray.QSort(CmpRM1);
    // for (int i=0; i < ind; i++)
    // {
    //     fprintf(stderr,"i: %d, rmArray: %d %d %d \r\n", i, \
    //     rmArray[i].nSlot, rmArray[i].nBucket, rmArray[i].storageCost);
    // }
    // getchar();

    // int nBucketStart = 16384;
    // int nBucketEnd = 16384; // this number must be larger than 4
    // // start with   minimum number slots
    // int nSlotStart = 1024;
    // int nSlotEnd = 1024;

    int minCost = nSlotEnd*11 + nBucketEnd*2 + 1;
    int bestBucketSize = nBucketEnd + 1;
    int bestSlotSize   = nSlotEnd + 1;
    uint32_t rBest;
    uint32_t mBest;
    TArray<int> dArrayBest;
    TArray<int> bucketArrayBest;

    // time calculation
	struct timeval gStart, gBegin, gEnd;
    double timeuse = -1;
    gettimeofday(&gStart,NULL);

    TPerfectRouterBuilder1 myB;

    for (int i=0; i < ind; i++)
    {
        int nSlot = rmArray[i].nSlot;
        int nBucket = rmArray[i].nBucket;
        int curCost = nSlot*11 + nBucket*2;
        // if (curCost < minCost)
        // {
            gettimeofday(&gBegin,NULL);
            // build the current router function with r, m
            bool success = myB.Build(pn, hash, nHash, nBucket, nSlot);
            if (success) {
                minCost        = curCost;
                bestBucketSize = nBucket;
                bestSlotSize   = nSlot;
                rBest = myB.r; // same as bestSlotSize
                mBest = myB.m; // same as bestBucketSize
                dArrayBest.Close();
                dArrayBest.FastCopy(myB.dArray);
                bucketArrayBest.Close();
                bucketArrayBest.FastCopy(myB.bucketArray);
                gettimeofday(&gEnd,NULL);
                timeuse = gEnd.tv_sec - gBegin.tv_sec + (gEnd.tv_usec - gBegin.tv_usec)/1000000.0;
                // fprintf(stderr,"success!!!  \r\n");
                // fprintf(stderr,"num of slots: %d \r\n", myB.r);
                // fprintf(stderr,"num of buckets, nBucket: %d \r\n", myB.m);
                // fprintf(stderr,"bit memory cost = 11 bit x %d(nSlot) + 2 bit x %d(nBucket) = %d \r\n", myB.r, myB.m, minCost);
                storageCost = minCost;
                if(maxStorageCost < storageCost){
                    // fprintf(stderr,"Current storage cost is: %d bit\n", storageCost);
                    // fprintf(stderr,"Current maxStorageCost cost is: %d bit\n", maxStorageCost);
                    // fprintf(stderr,"Current maxStorageCost cost is: %u bit\n", maxStorageCost);
                    maxStorageCost = storageCost;
                }
                rmArray.Close();
                myB.Close();
                return timeuse;
            }
        // }
    }

    if (bestBucketSize == nBucketEnd+1)
    {
        fprintf(stderr,"!!!!! failure for current key set file, please check the Generator !!!!!\r\n");
    }
    myB.Close();
    rmArray.Close();
    return timeuse;
}

double CTimer1::getFileTime(const char * filename, int *pn, uint64_t & maxStorageCost)
{
    uint32_t key[10000];
    int ind = 0;
    ifstream input(filename, ios::in);
	string line;
	while (getline(input, line)) {
        string l = line.data();
        int len = line.length();        //strlen(line);
        if (len == 0) {
            fprintf(stderr,"no data in current line, check %s.\r\n", filename);
            return 0;
        }
        else if (len > 11) {      // check the length of IP address
            fprintf(stderr,"string %s is too long \r\n",l.c_str());
            return 0;
        }
        uint32_t h = atoi(l.c_str());
        key[ind++] = h; // load the string hash value
	}
    int nHash = ind;

    int nHash1 = (int)(sizeof(key) / sizeof(key[0]));
    if (nHash != nHash1) {
        fprintf(stderr,"num of input keys doesn't match. \r\n");
        fprintf(stderr,"nHash: %d\r\n", nHash);
        fprintf(stderr,"nHash1: %d\r\n", nHash1);
        fprintf(stderr,"check file %s.\r\n", filename);
        return 0;
    }

    uint32_t *hash = key;
    double timeuse;
    timeuse = getCTime(hash, pn, nHash, maxStorageCost);
    
    return timeuse;
}

//------------------------------------------------------------------------------
