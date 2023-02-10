#include <stdio.h>
#include <cstring>
#include <fstream>
#include <sys/time.h>
#include "CTimer2.h"

//------------------------------------------------------------------------------
using namespace std;

double CTimer2::getCTime(uint32_t *hash, int *pn,int nHash, uint64_t &maxStorageCost)
{
    ///////////////////////////////////
    // we fix the nBucket and nSlot to compare construction time
    // start with maximum number of buckets
    int bucketStart = 8192;
    int bucketEnd = 8192; // this number must be larger than 4
    // start with   minimum number slots
    int nSlotStart = 1024;
    int nSlotEnd = 1024;
    
    int slotWidth = 15;
    int minCost = nSlotEnd*slotWidth  + bucketStart*2 + 1;
    int bestBucketSize = bucketStart+1;
    int bestSlotSize   = nSlotStart;

    // number of buckets
    int nBucket;
    int nSlot = nSlotStart;

    TArray<int> dArrayBest;
    TArray<int> bucketArrayBest;

    // time calculation
	struct timeval gStart, gBegin, gEnd;
    double timeuse = -1;
    gettimeofday(&gStart,NULL);

    TPerfectRouterBuilder2 myB;
    while (nSlot <= nSlotEnd)
    {
        nBucket = bucketStart;
        while(nBucket >= bucketEnd)
        {
            // fprintf(stderr,"nslots: %d \r\n", nSlot);
            // fprintf(stderr,"nbuckets: %d \r\n", nBucket);
            // number of slots
            int curCost = nSlot*slotWidth  + nBucket*2;
            if (curCost < minCost)
            {
                gettimeofday(&gBegin,NULL);
                // build the current router function with r, m
                bool success = myB.Build(pn, hash, nHash, nBucket, nSlot);
                if (success) {
                    minCost        = curCost;
                    bestBucketSize = nBucket;
                    bestSlotSize   = nSlot;
                    dArrayBest.Close();
                    dArrayBest.FastCopy(myB.dArray);
                    bucketArrayBest.Close();
                    bucketArrayBest.FastCopy(myB.bucketArray);
                    gettimeofday(&gEnd,NULL);
                    timeuse = gEnd.tv_sec - gBegin.tv_sec + (gEnd.tv_usec - gBegin.tv_usec)/1000000.0;
                    fprintf(stderr,"success!!!  \r\n");
                    fprintf(stderr,"num of slots: %d \r\n", myB.r);
                    fprintf(stderr,"num of buckets, nBucket: %d \r\n", myB.m);
                    fprintf(stderr,"bit memory cost =   %d  bit x nSlot + 2 bit x nBucket = %d \r\n", slotWidth, minCost);
                    uint64_t storageCost = (uint64_t) minCost;
                    if(maxStorageCost < storageCost){
                        maxStorageCost = storageCost;
                    }

                    return timeuse;
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
    myB.Close();
    return timeuse;
}

double CTimer2::getFileTime(const char * filename, uint64_t &maxStorageCost)
{
    uint32_t key[10000];
    int port[10000];
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
    
    // get port number, default 1-4
    int count = 0;
    while ( count < nHash ) {
        if (count < (int)(nHash/4))
            port[count++] = 1;
        else if (count < (int)(nHash/2))
            port[count++] = 2;
        else if (count < (int)(nHash*3/4))
            port[count++] = 3;
        else
            port[count++] = 4;
    }

    uint32_t *hash = key;
    int *pn = port;
    double timeuse;
    timeuse = getCTime(hash, pn, nHash, maxStorageCost);
    
    return timeuse;
}

//------------------------------------------------------------------------------
