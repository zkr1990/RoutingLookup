#include "BooPHF.h"

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/io.h>
#include <sys/resource.h>
#include <random>
#include <algorithm>

#include <dirent.h>

using namespace std;

vector<string> filePath;

typedef boomphf::SingleHashFunctor<u_int64_t>  hasher_t;
typedef boomphf::mphf<  u_int64_t, hasher_t  > boophf_t;


double generate(const char * filename)
{
    //tells bbhash to use included hash function working on u_int64_t input keys :
	typedef boomphf::SingleHashFunctor<u_int64_t>  hasher_t;
	typedef boomphf::mphf<  u_int64_t, hasher_t  > boophf_t;

	std::vector<u_int64_t> input_keys;
	uint nthreads = 1;

	// import from input file
    // define hash array for input hash keys
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
        u_int64_t h = atoi(l.c_str());
        input_keys.push_back(h); // load the string hash value
	}
	
	//build the mphf    
	struct timeval gStart, gEnd;
    double timeuse = -1;
    gettimeofday(&gStart,NULL);
    double gammaFactor = 5.0;
	boophf_t * bphf = new boomphf::mphf<u_int64_t,hasher_t>(input_keys.size(),input_keys,nthreads,gammaFactor);
	gettimeofday(&gEnd,NULL);
    timeuse = gEnd.tv_sec - gStart.tv_sec + (gEnd.tv_usec - gStart.tv_usec)/1000000.0;
    // fprintf(stderr,"Use Time:%f\r\n",timeuse);

	//query the mphf example:
	// uint64_t  idx = bphf->lookup(input_keys[0]);

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

int main (int argc, char* argv[]){
	
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
	for (int i=0;i<fileNum;i++)
	{
		// fprintf(stderr,"%s\n", filePath[i].c_str());
		const char *curFileP = filePath[i].c_str();
		double curTime = generate(curFileP);
		accTime += curTime;
		if (curTime > maxTime){
			maxTime = curTime;
		}
	}
	fprintf(stderr,"Total time for all key sets is: %.09f sec\n", accTime);
	fprintf(stderr,"avg time for one key set is: %.012f sec\n", accTime/fileNum);
	fprintf(stderr,"max time for all key set is: %.012f sec\n", maxTime);
	return 1;
}
