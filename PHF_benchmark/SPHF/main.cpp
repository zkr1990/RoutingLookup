#include <stdio.h>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
// #include <sys/io.h>
#include <inttypes.h>
#include <math.h>
#include <sys/time.h>

#include <dirent.h>

#include "IntUtils.h"
#include "FastStrHash.h"
#include "PerfectHash.h"
#include "PerfectHashBuilder.h"
#include "PerfectHashBuilder.h"
//------------------------------------------------------------------------------
using namespace std;

vector<string> filePath;

// struct timeval{
// long tv_sec;
// long tv_usec;
// };

double generate(const char * filename,  uint64_t &maxStorageCost)
{
    // define hash array for input hash keys
    vector<uint64_t> inputKey;
    int ind = 0;
    // import from input file
    // const char * filename = (argv[1]);
    ifstream input(filename, ios::in);
	string line;
    // fprintf(stderr,"results are:\n");
	while (getline(input, line)) {
        const char *l = line.data();
        int len = line.length();        //strlen(line);
        if (len == 0) {
            fprintf(stderr,"no data in current line.\r\n");
            return 0;
        }
        else if (len > 11) {      // check the length of IP address
            fprintf(stderr,"string '%s' is too short or too long \r\n",line.c_str());
            return 0;
        }
        uint64_t h = FastStrHash(l, len);
        // fprintf(stderr,"%lu\n",h);
        //key[ind++] = h; // load the string hash value
	    inputKey.push_back(h);
	}

    // fprintf(stderr,"finish loading hash keys \r\n");
    uint64_t key[inputKey.size()];
    for (int i=0;i<inputKey.size();i++){
        key[i]=inputKey[i];
    }
    uint64_t *hash = key;
    uint64_t nHash = inputKey.size();
    int spaceExt = 1;
    bool ext = false;                   // extend bucket space or not? default is no
    int extSpace = (ext?8:1);
	
	
	struct timeval gStart, gBegin, gEnd;
    double timeuse = -1;
    gettimeofday(&gStart,NULL);
	// generate phase
    for(int bucketExtr=0;bucketExtr<=(extSpace==1?20:100);bucketExtr+=4)
    {
        // fprintf(stderr,"bucketExtr: %d \r\n", bucketExtr);
        // fprintf(stderr,"-----------------------\r\n");
        nHash = (uint64_t)(sizeof(key) / sizeof(key[0]));
        uint64_t nBucket    = RoundUpPrime(nHash*(100+bucketExtr)/100);
        uint64_t nSpace     = nBucket*spaceExt;
        for(int nHashPerSlot=(sizeof(TPerfectHashData::d[0])*8-1);nHashPerSlot>1;nHashPerSlot--)
        {
            int nPerSlot      = nHashPerSlot*100;
            
            TPerfectHashBuilder myB;
            gettimeofday(&gBegin,NULL);
            bool success = myB.Build(hash, nHash, nBucket, nPerSlot, nSpace);
            if (success) {
                // fprintf(stderr,"success!!!  \r\n");
                // fprintf(stderr,"nHashPerSlot: %d \r\n", nHashPerSlot);
                // // fprintf(stderr,"num of hash space per slot, nPerSlot: %d \r\n", nPerSlot);
                // fprintf(stderr,"num of hash keys, nHash: %lu \r\n", nHash);
                // fprintf(stderr,"num of buckets, nBucket: %lu \r\n", nBucket);
                // fprintf(stderr,"num of slots: %lu \r\n", RoundUpPrime(nHash*100/nPerSlot));
                // fprintf(stderr,"Use Time:%f\r\n",timeuse);
                // Now, current myB contains the best parameters, output to files
                
                // ofstream output;
                // string outFileName = "function.txt";
                // output.open(outFileName);
                // output << nBucket << endl;
                // output << nPerSlot << endl;
                // // output each d value in d[](dArrayBest)
                // for (int slot=0;slot<myB.data.Number;slot++)
                // {
                //     output << myB.data[slot] << endl;
                // }
                gettimeofday(&gEnd,NULL);
                timeuse = gEnd.tv_sec - gBegin.tv_sec + (gEnd.tv_usec - gBegin.tv_usec)/1000000.0;
                fprintf(stderr,"nBucket: %d \r\n", nBucket);
		uint64_t storageCost = (uint64_t) 16 * myB.data.Number;
                fprintf(stderr,"SlotSize: %d, bit memory cost = 16 bit x nSlot: %d \r\n", 
                    myB.data.Number, storageCost);
		if(maxStorageCost < storageCost){
			maxStorageCost = storageCost;
		}
                return timeuse;
            } 
            // else {
            //     fprintf(stderr,"----------------------------- \r\n");
            // }
        }
    }
    fprintf(stderr,"fail. \r\n");
    getchar();
    return timeuse;
}


int listdir(string path) {
    struct dirent *entry;
    DIR *dp;

    dp = opendir(path.c_str());
    if (dp == NULL) {
        perror("opendir: Path does not exist or could not be read.");
        return -1;
    }

    while ((entry = readdir(dp)))
	{
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			continue;
		else if (entry->d_type == 8)
		{
			string filePathName = (path + entry->d_name);
			// fprintf(stderr,"%s\n", filePathName);
			filePath.push_back(filePathName);
		}
		else if (entry->d_type == 10)
			continue;
		else if (entry->d_type == 4) // this is a folder, not a file
		{
			listdir(path + entry->d_name + "/");
		}
		
	}		

    closedir(dp);
    return 0;
}

//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    // make sure two args
    if( (argc<2) || (argc>2) )
    {
        fprintf(stderr,"Check hash  : Generate folderName\r\n");
        return 1;
    }

    string path = argv[1];
    listdir(path);

	int fileNum = filePath.size();
	double accTime = 0.0;
	double maxTime = 0.0;
	
	uint64_t maxStorageCost = 0;
	//maxStorageCost = 0;
	for (int i=0;i<fileNum;i++)
	{
		// fprintf(stderr,"%s\n", filePath[i].c_str());
		const char *curFileP = filePath[i].c_str();
		double curTime = generate(curFileP, maxStorageCost);
		accTime += curTime;
		if (curTime > maxTime){
			maxTime = curTime;
		}
	}
	fprintf(stderr,"Maximum storage cost is: %d bit\n", maxStorageCost);
	fprintf(stderr,"Total time for all key sets is: %.09f sec\n", accTime);
	fprintf(stderr,"avg time for one key set is: %.012f sec\n", accTime/fileNum);
	fprintf(stderr,"max time for all key set is: %.012f sec\n", maxTime);
	return 1;
}
//------------------------------------------------------------------------------