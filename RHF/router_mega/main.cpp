#include <stdio.h>
#include <cstring>
#include <fstream>
#include <math.h>
#include <unistd.h>
#include <ios>
#include <vector>
#include <dirent.h>
#include <sys/io.h>
#include <sys/resource.h>
#include <sys/time.h>

#include "Generator.h"
#include "Tester/PerfectRouterTester1.h"
#include "Tester/PerfectRouterTester2.h"
#include "Tester/PerfectRouterTester3.h"
#include "CTimer1.h"
#include "CTimer2.h"
#include "CTimer3.h"
// #include "Performance/Performance.h"

//------------------------------------------------------------------------------
using namespace std;
vector<string> filePath;
// Resource memory calculation
// typedef struct {
//     unsigned long size,resident,share,text,lib,data,dt;
// } statm_t;
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
// statm_t getCurrentRSS()
// {
//     statm_t result;
//     FILE* fp = NULL;
//     if ( (fp = fopen( "/proc/self/statm", "r" )) == NULL )
//     {
//         fprintf(stderr,"can not open /proc/self/statm \r\n");
//         return result;
//     }
//     if(7 != fscanf(fp,"%ld %ld %ld %ld %ld %ld %ld",
//     &result.size,&result.resident,&result.share,&result.text,&result.lib,&result.data,&result.dt))
//     {
//         fprintf(stderr,"wrong information in /proc/self/statm \r\n");
//         return result;
//     }
//     fclose( fp );
//     return result; // in pages
// }

// int RoundUpPow2(int val)
// {
//     int base2 = 2;
//     while( base2 < val)
//     {
//         base2 *= 2;
//     }
//     return base2;
// }

//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    // struct rusage start, end, gStart, gEnd;
    // getrusage(RUSAGE_SELF, &start);
    // make sure two args
    if( (argc<3) || (argc>5) )
    {
        fprintf(stderr,"%d", argc);
        fprintf(stderr,"Check command:\r\n");
        fprintf(stderr,"Please specify the router n = 1/2/3\r\n");

        fprintf(stderr,"Report construction time:   ./Generate n FolderPath\r\n");
        fprintf(stderr,"    terminal print time information.\r\n");
        fprintf(stderr,"Generate function:          ./Generate n input.txt function.txt\r\n");
        fprintf(stderr,"    function.txt will be generated.\r\n");
        fprintf(stderr,"Test generated function:    ./Generate n input.txt function.txt -t\r\n");
        fprintf(stderr,"    function.txt will be tested based on input.txt\r\n");
        return 1;
    }
    // Check n is 1/2/3
    if ((strcmp(argv[1],"1")!=0) && strcmp(argv[1],"2")!=0 && strcmp(argv[1],"3")!=0)
    {
        fprintf(stderr,"Please specify the router n = 1/2/3\r\n");
        return 1;
    }
    
    ////////////////////////////////////////////////////////////////////////
    // Add more checking logic for bad input args
    ////////////////////////////////////////////////////////////////////////
    if (argc==3) // compare construction time
    {
        string path = argv[2];
        listdir(path);

        int fileNum = filePath.size();
        double accTime = 0.0;
        double maxTime = 0.0;
	    uint64_t maxStorageCost = 0;
        for (int i=0;i<fileNum;i++)
        {
            double curTime;
            // fprintf(stderr,"%s\n", filePath[i].c_str());
            const char *curFileP = filePath[i].c_str();
            if(strcmp(argv[1],"1")==0) {
                CTimer1 Timer;
                curTime = Timer.getFileTime(curFileP, maxStorageCost);
                Timer.Close();
            } else if(strcmp(argv[1],"2")==0) {
                CTimer2 Timer;
                curTime = Timer.getFileTime(curFileP, maxStorageCost);
                Timer.Close();
            } else if(strcmp(argv[1],"3")==0) {
                CTimer3 Timer;
                curTime = Timer.getFileTime(curFileP, maxStorageCost);
                Timer.Close();
            }
            
            accTime += curTime;
            if (curTime > maxTime){
                maxTime = curTime;
            }
        }
	    fprintf(stderr,"Maximum storage cost is: %d bit\n", maxStorageCost);
        fprintf(stderr,"Total time for all key sets is: %.09f sec\n", accTime);
        fprintf(stderr,"avg time for one key set is: %.012f sec\n", accTime/fileNum);
        fprintf(stderr,"max time for all key set is: %.012f sec\n", maxTime);
        return 0;
    }

    // import from input file
    const char * inFileName = (argv[2]);
    // define hash array for input hash keys
    uint32_t key[10000];
    int port[10000];
    int ind = 0;
    ifstream input(inFileName, ios::in);
	string line;
	while (getline(input, line)) {
        string l = line.data();
        int len = line.length();        //strlen(line);
        if (len == 0) {
            fprintf(stderr,"no data in current line, check %s.\r\n", inFileName);
            return 0;
        }
        else if (len > 11) {      // check the length of IP address
            fprintf(stderr,"string %s is too long \r\n",l.c_str());
            return 0;
        }
        uint32_t h = atoi(l.c_str());
        key[ind++] = h; // load the string hash value
	}

    // fprintf(stderr,"Hash keys loaded for file: %s\r\n", inFileName);
    int nHash = ind;
    int nHash1 = (int)(sizeof(key) / sizeof(key[0]));
    if (nHash != nHash1) {
        fprintf(stderr,"num of input keys doesn't match. \r\n");
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
    // Test the correctness generated function for Interface Mapping
    // or, Generate the function
    const char *funcFileName = argv[3];
    if (argc==4) // generate function
    {
        if(strcmp(argv[1],"1")==0) {
            bool gFinish = generateFunction1(argv[3], hash, pn, nHash);
        } else if(strcmp(argv[1],"2")==0) {
            bool gFinish = generateFunction2(argv[3], hash, pn, nHash);
        } else if(strcmp(argv[1],"3")==0) {
            bool gFinish = generateFunction3(argv[3], hash, pn, nHash);
        }
    } else if((argc==5) && strcmp(argv[4],"-t")==0) // verify function
    {
        if(strcmp(argv[1],"1")==0) {
            TPerfectRouterTester1 myT;
            bool tFinish = myT.Test(funcFileName, hash, pn, nHash);
            myT.Close();
        } else if(strcmp(argv[1],"2")==0) {
            TPerfectRouterTester2 myT;
            bool tFinish = myT.Test(funcFileName, hash, pn, nHash);
            myT.Close();
        } else if(strcmp(argv[1],"3")==0) {
            TPerfectRouterTester3 myT;
            bool tFinish = myT.Test(funcFileName, hash, pn, nHash);
            myT.Close();
        }
    }
    // fprintf(stderr,"pageSize: %ld kB\r\n", (size_t)(sysconf( _SC_PAGESIZE)/1024));
    // Get statistics about the performance
    // getrusage(RUSAGE_SELF, &end);
    // fprintf(stderr,"Peak resident set size: %ld kB\n", end.ru_maxrss);

    // statm_t result = getCurrentRSS();
    // long pageKB = (long)(sysconf( _SC_PAGESIZE)/1024);
    // fprintf(stderr,"resident set (VmRSS) size: %lu kB\r\n", result.resident * pageKB);
    // fprintf(stderr,"virtual memory (VmSize) usage: %lu kB\r\n", result.size * pageKB);
    // fprintf(stderr,"share memory (RssFile+RssShmem) usage: %lu kB\r\n", result.share * pageKB);
    // fprintf(stderr,"code text usage: %lu kB\r\n", result.text * pageKB);
    // fprintf(stderr,"data + stack usage: %lu kB\r\n", result.data * pageKB);
    
    return 0;
}
//------------------------------------------------------------------------------
