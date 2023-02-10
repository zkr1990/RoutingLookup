
#include <stdio.h>
#include <fstream>
#include "Array.h"
#include "Builder/PerfectRouterBuilder1.h"
#include "Builder/PerfectRouterBuilder2.h"
#include "Builder/PerfectRouterBuilder3.h"
//------------------------------------------------------------------------------
using namespace std;

bool generateFunction1(const char *funcFileName, uint32_t *hash, int *pn,int nHash)
{
    // // start with maximum number of buckets
    // int bucketStart = 16384;
    // int bucketEnd = 2048; // this number must be larger than 4
    // // start with   minimum number slots
    // int nSlotStart = 512;
    // int nSlotEnd = 4096;
    // start with maximum number of buckets
    int bucketStart = 16384;
    int bucketEnd = 16384; // this number must be larger than 4
    // start with   minimum number slots
    int nSlotStart = 1024;
    int nSlotEnd = 1024;

    int slotWidth = 11;
    int minCost = nSlotStart*slotWidth + bucketStart*2 + 1;
    int bestBucketSize = bucketStart-1;
    int bestSlotSize   = nSlotStart;

    int nBucket;
    int nSlot = nSlotStart;

    uint32_t rBest;
    uint32_t mBest;
    TArray<int> dArrayBest;
    TArray<int> bucketArrayBest;

    TPerfectRouterBuilder1 myB;
    while (nSlot <= nSlotEnd)
    {
        nBucket  = bucketStart;
        while(nBucket >= bucketEnd)
        {
            // fprintf(stderr,"nslots: %d \r\n", nSlot);
            // fprintf(stderr,"nbuckets: %d \r\n", nBucket);
            int curCost = nSlot*slotWidth + nBucket*2;
            if (curCost < minCost)
            {
                // build the current router function with r, m
                bool success = myB.Build(pn, hash, nHash, nBucket, nSlot);
                if (success) {
                    minCost        = curCost;
                    bestBucketSize = nBucket;
                    bestSlotSize   = nSlot;
                    // fprintf(stderr,"success!!!  \r\n");
                    // fprintf(stderr,"nHashPerSlot: %d \r\n", nHashPerSlot);
                    // fprintf(stderr,"num of hash keys, nHash: %d \r\n", nHash);
                    // fprintf(stderr,"num of slots: %d \r\n", myB.r);
                    // fprintf(stderr,"num of buckets, nBucket: %d \r\n", myB.m);
                    rBest = myB.r; // same as bestSlotSize
                    mBest = myB.m; // same as bestBucketSize
                    dArrayBest.Close();
                    dArrayBest.FastCopy(myB.dArray);
                    bucketArrayBest.Close();
                    bucketArrayBest.FastCopy(myB.bucketArray);
                }
            }
            nBucket = (int) nBucket / 2;
        }
        nSlot = (int) nSlot * 2;
    }
    if (bestBucketSize == bucketStart-1)
    {
        fprintf(stderr,"!!!!! failure for current key set file, please check the Generator !!!!!\r\n");
    }
    // Now, current myB contains the best parameters, output to files
    ofstream output;
    string outFileName = funcFileName;
    output.open(outFileName);
    output << rBest << endl;
    output << mBest << endl;
    // output each d value in d[](dArrayBest)
    for (int slot=0;slot<rBest;slot++)
    {
        output << dArrayBest[slot] << endl;
    }
    // output each bucket interface number in bucketArrayBest
    for (int buck=0;buck<mBest;buck++)
    {
        output << bucketArrayBest[buck] << endl;
    }
    output.close();
    // Close the builder
    myB.Close();
    fprintf(stderr,"bestBucketSize: %d, bestSlotSize: %d\r\n", bestBucketSize, bestSlotSize);
    fprintf(stderr,"bit memory cost = %d bit x nSlot + 2 bit x nBucket = %d \r\n", slotWidth, minCost);

    // fprintf(stderr,"finish generating functions ------------------------. \r\n");
    fprintf(stderr,"\r\n");
    return true;
}

bool generateFunction2(const char *funcFileName, uint32_t *hash, int *pn,int nHash)
{
    // // start with maximum number of buckets
    // int bucketStart = 16384;
    // int bucketEnd = 4096; // this number must be larger than 4
    // // start with   minimum number slots
    // int nSlotStart = 512;
    // int nSlotEnd = 1024;
    // start with maximum number of buckets
    int bucketStart = 8192;
    int bucketEnd = 8192; // this number must be larger than 4
    // start with   minimum number slots
    int nSlotStart = 1024;
    int nSlotEnd = 1024;
    
    int slotWidth = 15;
    int bestBucketSize = bucketStart+1;
    int bestSlotSize   = nSlotStart;

    // number of buckets
    int nBucket  = bucketStart;
    int nSlot = nSlotStart;
    int minCost = nSlot*slotWidth  + nBucket*2 + 1;

    uint32_t rBest;
    uint32_t mBest;
    TArray<int> dArrayBest;
    TArray<int> bucketArrayBest;

    TPerfectRouterBuilder2 myB;
    while (nSlot <= nSlotEnd)
    {
        nBucket = bucketStart;
        while(nBucket >= bucketEnd)
        {
            fprintf(stderr,"nslots: %d \r\n", nSlot);
            fprintf(stderr,"nbuckets: %d \r\n", nBucket);
            // number of slots
            int curCost = nSlot*slotWidth  + nBucket*2;
            if (curCost < minCost)
            {
                // build the current router function with r, m
                bool success = myB.Build(pn, hash, nHash, nBucket, nSlot);
                if (success) {
                    minCost        = curCost;
                    bestBucketSize = nBucket;
                    bestSlotSize   = nSlot;
                    fprintf(stderr,"success!!!  \r\n");
                    // fprintf(stderr,"nHashPerSlot: %d \r\n", nHashPerSlot);
                    // fprintf(stderr,"num of hash keys, nHash: %d \r\n", nHash);
                    fprintf(stderr,"num of slots: %d \r\n", myB.r);
                    fprintf(stderr,"num of buckets, nBucket: %d \r\n", myB.m);
                    rBest = myB.r; // same as bestSlotSize
                    mBest = myB.m; // same as bestBucketSize
                    dArrayBest.Close();
                    dArrayBest.FastCopy(myB.dArray);
                    bucketArrayBest.Close();
                    bucketArrayBest.FastCopy(myB.bucketArray);
                    break;
                }
            }
            nBucket = (int) nBucket / 2;
        }
        nSlot = (int) nSlot * 2;
    }
    if (bestBucketSize == bucketStart+1)
    {
        fprintf(stderr,"!!!!! failure for current key set file, please check the Generator !!!!!\r\n");
    }
    // Now, current myB contains the best parameters, output to files
    ofstream output;
    string outFileName = funcFileName;
    output.open(outFileName);
    output << rBest << endl;
    output << mBest << endl;
    // output each d value in d[](dArrayBest)
    for (int slot=0;slot<rBest;slot++)
    {
        output << dArrayBest[slot] << endl;
    }
    // output each bucket interface number in bucketArrayBest
    for (int buck=0;buck<mBest;buck++)
    {
        output << bucketArrayBest[buck] << endl;
    }
    output.close();
    // Close the builder
    myB.Close();
    fprintf(stderr,"bestBucketSize: %d, bestSlotSize: %d\r\n", bestBucketSize, bestSlotSize);
    fprintf(stderr,"bit memory cost = %d bit x nSlot + 2 bit x nBucket = %d \r\n", slotWidth, minCost);

    // fprintf(stderr,"finish generating functions ------------------------. \r\n");
    fprintf(stderr,"\r\n");
    return true;
}

bool generateFunction3(const char *funcFileName, uint32_t *hash, int *pn,int nHash)
{
    // // start with maximum number of buckets
    // int bucketStart = 16384;
    // int bucketEnd = 2048; // this number must be larger than 4
    // // start with   minimum number slots
    // int nSlotStart = 512;
    // int nSlotEnd = 4096;
    // start with maximum number of buckets
    int bucketStart = 32768;
    int bucketEnd = 32768; // this number must be larger than 4
    // start with   minimum number slots
    int nSlotStart = 2048;
    int nSlotEnd = 2048;

    int slotWidth = 8;
    int minCost = nSlotStart*slotWidth + bucketStart*2 + 1;
    int bestBucketSize = bucketStart-1;
    int bestSlotSize   = nSlotStart;

    int nBucket;
    int nSlot = nSlotStart;

    uint32_t rBest;
    uint32_t mBest;
    TArray<int> dArrayBest;
    TArray<int> bucketArrayBest;

    TPerfectRouterBuilder3 myB;
    while (nSlot <= nSlotEnd)
    {
        nBucket  = bucketStart;
        while(nBucket >= bucketEnd)
        {
            // fprintf(stderr,"nslots: %d \r\n", nSlot);
            // fprintf(stderr,"nbuckets: %d \r\n", nBucket);
            int curCost = nSlot*slotWidth + nBucket*2;
            if (curCost < minCost)
            {
                // build the current router function with r, m
                bool success = myB.Build(pn, hash, nHash, nBucket, nSlot);
                if (success) {
                    minCost        = curCost;
                    bestBucketSize = nBucket;
                    bestSlotSize   = nSlot;
                    // fprintf(stderr,"success!!!  \r\n");
                    // fprintf(stderr,"nHashPerSlot: %d \r\n", nHashPerSlot);
                    // fprintf(stderr,"num of hash keys, nHash: %d \r\n", nHash);
                    // fprintf(stderr,"num of slots: %d \r\n", myB.r);
                    // fprintf(stderr,"num of buckets, nBucket: %d \r\n", myB.m);
                    rBest = myB.r; // same as bestSlotSize
                    mBest = myB.m; // same as bestBucketSize
                    dArrayBest.Close();
                    dArrayBest.FastCopy(myB.dArray);
                    bucketArrayBest.Close();
                    bucketArrayBest.FastCopy(myB.bucketArray);
                }
            }
            nBucket = (int) nBucket / 2;
        }
        nSlot = (int) nSlot * 2;
    }
    if (bestBucketSize == bucketStart+1)
    {
        fprintf(stderr,"!!!!! failure for current key set file, please check the Generator !!!!!\r\n");
    }
    // Now, current myB contains the best parameters, output to files
    ofstream output;
    string outFileName = funcFileName;
    output.open(outFileName);
    output << rBest << endl;
    output << mBest << endl;
    // output each d value in d[](dArrayBest)
    for (int slot=0;slot<rBest;slot++)
    {
        output << dArrayBest[slot] << endl;
    }
    // output each bucket interface number in bucketArrayBest
    for (int buck=0;buck<mBest;buck++)
    {
        output << bucketArrayBest[buck] << endl;
    }
    output.close();
    // Close the builder
    myB.Close();
    fprintf(stderr,"bestBucketSize: %d, bestSlotSize: %d\r\n", bestBucketSize, bestSlotSize);
    fprintf(stderr,"bit memory cost = %d bit x nSlot + 2 bit x nBucket = %d \r\n", slotWidth, minCost);

    // fprintf(stderr,"finish generating functions ------------------------. \r\n");
    fprintf(stderr,"\r\n");
    return true;
}
