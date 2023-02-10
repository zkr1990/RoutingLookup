#include <stdio.h>
#include <cstring>
#include <fstream>
#include <cmath>
#include <sys/time.h>
#include "CTimer3.h"

//------------------------------------------------------------------------------
using namespace std;

double CTimer3::getCTime(uint64_t *hash, int *pn,int nHash, uint64_t &maxStorageCost)
{
    ///////////////////////////////////
    // we fix the nBucket and nSlot to compare construction time
    // start with maximum number of buckets
    int bucketStart = 32768;
    int bucketEnd = 32768; // this number must be larger than 4
    // start with   minimum number slots
    int nSlotStart = 1024;
    int nSlotEnd = 1024;
    
    int slotWidth = 8;
    int minCost = nSlotEnd*slotWidth + bucketStart*2 + 1;
    int bestBucketSize = bucketStart-1;
    int bestSlotSize   = nSlotStart;

    int nBucket;
    int nSlot = nSlotStart;

    uint32_t rBest;
    uint32_t mBest;
    TArray<int> dArrayBest;
    TArray<int> bucketArrayBest;

    // time calculation
	struct timeval gStart, gBegin, gEnd;
    double timeuse = -1;
    gettimeofday(&gStart,NULL);

    TPerfectRouterBuilder3 myB;
    while (nSlot <= nSlotEnd)
    {
        nBucket  = bucketStart;
        while(nBucket >= bucketEnd)
        {
            // fprintf(stderr,"nslots: %d \r\n", nSlot);
            // fprintf(stderr,"nbuckets: %d \r\n", nBucket);
            int curCost = nSlot*slotWidth + nBucket*2;
            // avoid some unuseful builds
            if (curCost < minCost)
            {
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
                    fprintf(stderr,"success!!!  \r\n");
                    fprintf(stderr,"num of slots: %d \r\n", myB.r);
                    fprintf(stderr,"num of buckets, nBucket: %d \r\n", myB.m);
                    fprintf(stderr,"bit memory cost = %d bit x nSlot + 2 bit x nBucket = %d \r\n", slotWidth, minCost);
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
        // nSlot = (int) nSlot * 2;
    }
    if (bestBucketSize == bucketStart-1)
    {
        fprintf(stderr,"!!!!! failure for current key set file, please check the Generator !!!!!\r\n");
    }
    myB.Close();
    return timeuse;
}

double CTimer3::getFileTime(const char * filename, uint64_t & maxStorageCost)
{
    uint64_t key[10000];
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
        else if (len > 20) {      // check the length of IP address
            fprintf(stderr,"string %s is too long \r\n",l.c_str());
            return 0;
        }
        uint64_t h = strtoull(l.c_str(), NULL, 10);
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

    uint64_t *hash = key;
    int *pn = port;
    double timeuse;
    timeuse = getCTime(hash, pn, nHash, maxStorageCost);
    
    return timeuse;
}

//------------------------------------------------------------------------------
