//---------------------------------------------------------------------------
#include <stdio.h>
#include <cstring>
#include <fstream>
#include <sys/io.h>
#include <sys/resource.h>
#include <unistd.h>
#include <ios>

#include "PerfectRouterTester1.h"
#include "../Performance/Performance.h"
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
// bool checkFunc(const int *pn, const uint32_t *hash, const int nHash, const int nSlot, const int nBucket, TArray<int> TdArray, TArray<int> TbucketArray)
// {
// 	int bucketWidth = (int) (log(nBucket)/log(2));
// 	int shiftBucket = 32 - bucketWidth;

//     int nSlotWidth = (int)(log(nSlot)/log(2));
//     int shiftSlot = 32 - nSlotWidth;
//     fprintf(stderr,"TdArray length: %d\r\n", TdArray.Number);
//     fprintf(stderr,"TbucketArray Number: %d\r\n", TbucketArray.Number);
    
//     for(int i=0;i<nHash;i++)
// 	{
//         // Step1: mod r， use bit-shift for mod operation
//         int idx = (hash[i] << shiftSlot) >> shiftSlot; // int idx = hash[i]%nSlot;
        
//         // Step2: slot lookup
//         fprintf(stderr,"hash[i]: %u\r\n", hash[i]);
// 		   fprintf(stderr,"nSlot: %d\r\n", nSlot);
//         fprintf(stderr,"shiftSlot: %d\r\n", shiftSlot);
//         fprintf(stderr,"idx: %d\r\n", idx);
//         int d = TdArray[idx];

//         // Step3: universal shift hash and mod m
//         uint32_t x_key = hash[i] >> 8;
//         int f = ( ((x_key * (uint32_t)d) >> 8) << shiftBucket >> shiftBucket);

//         // Step4: Interface number lookup
//         fprintf(stderr,"f: %d\r\n", f);
//         if (TbucketArray[f] != pn[i])
//         {
//             return false;
//         }
//     }
//     return true;
// }

bool TPerfectRouterTester1::Test(const char *funcFileName, uint32_t *hash, int *pn,int nHash)
{
	// get function paprameters from function.txt
    ifstream fInput(funcFileName, ios::in);
    string funcLine;
    // load r(nSlot) and m(nBucket)
    getline(fInput, funcLine);
    int nSlot = atoi(funcLine.c_str());
    getline(fInput, funcLine);
    int nBucket = atoi(funcLine.c_str());

    // initialize the array for TdArray[] and TbucketArray[]
    TdArray.AllocFill(24+nSlot*sizeof(int)); // 16bit x nSlot
    TdArray.InitZero();
    
    TbucketArray.AllocFill(24+nBucket*sizeof(int)); // 32bit x number of buckets
    TbucketArray.InitZero();

    // load slot values for TdArray[]
    for (int dIdx=0;dIdx<nSlot;dIdx++)
    {
        getline(fInput, funcLine);
        int len = funcLine.length();
        if (len == 0) {
            fprintf(stderr,"no data in current line %d, check %s.\r\n", dIdx+2, funcFileName);
            return 0;
        }
        TdArray[dIdx] = atoi(funcLine.c_str()); // load d value
    }

    // load bucket values for TbucketArray
    for (int bIdx=0;bIdx<nBucket;bIdx++)
    {
        getline(fInput, funcLine);
        int len = funcLine.length();
        if (len == 0) {
            fprintf(stderr,"no data in current line %d, check %s.\r\n", bIdx+nSlot+2, funcFileName);
            return 0;
        }
        TbucketArray[bIdx] = atoi(funcLine.c_str()); // load interface number in bucket
    }

    // check function for current key set
	int bucketWidth = (int) (log(nBucket)/log(2));
	int shiftBucket = 32 - bucketWidth;

    int nSlotWidth = (int)(log(nSlot)/log(2));
    int shiftSlot = 32 - nSlotWidth;
    // performance testing
    struct rusage tStart, tEnd;

    getrusage(RUSAGE_SELF, &tStart);
    for(int i=0;i<nHash;i++)
	{
        // Step1: mod r， use bit-shift for mod operation
        int idx = (hash[i] << shiftSlot) >> shiftSlot; // int idx = hash[i]%nSlot;
        
        // Step2: slot lookup
        int d = TdArray[idx];

        // Step3: universal shift hash and mod m
        uint32_t x_key = hash[i] >> 10;
        int f = ( ((x_key * (uint32_t)d) >> 8) << shiftBucket >> shiftBucket);
	
        // Step4: Interface number lookup
        if (TbucketArray[f] != pn[i])
        {
			fprintf(stderr,"###############################################  \r\n");
			fprintf(stderr,"testing fail for %s  \r\n", funcFileName);
			fprintf(stderr,"###############################################  \r\n");
			fInput.close();
            return false;
        }
    }
    getrusage(RUSAGE_SELF, &tEnd);
    
	fprintf(stderr,"Testing success for %s  \r\n", funcFileName);
    float tu = diffUserTime(&tStart, &tEnd);
    float ts = diffSystemTime(&tStart, &tEnd);
    float tTotal = tu + ts;
    fprintf(stderr,"Testing time for all keys:\n %.06f sec, %.06f user, %.06f system\n",
        tTotal, tu, ts);
	fInput.close();
    return true;
}

//---------------------------------------------------------------------------
