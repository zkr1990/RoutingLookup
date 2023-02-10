#include <stdio.h>
#include <cstring>
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <unistd.h>
#include <ios>
#include <vector>
#include <dirent.h>
#include <sys/io.h>
#include <sys/resource.h>
#include <sys/time.h>

#include "Generator.h"
#include "CTimer1.h"
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


//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    // struct rusage start, end, gStart, gEnd;
    // getrusage(RUSAGE_SELF, &start);
    // make sure two args
    if( (argc<3) || (argc>3) )
    {
        fprintf(stderr,"%d", argc);
        fprintf(stderr,"Check command:\r\n");
        fprintf(stderr,"Please specify the experiment n = 1/2\r\n");

        // fprintf(stderr,"Report construction time:   ./Generate n FolderPath\r\n");
        // fprintf(stderr,"    terminal print time information.\r\n");
        // experiment 1, 10000-4, different distributions
        // fprintf(stderr,"experiment 1, 10000-4, different distributions");
        // fprintf(stderr,"Generate function:          ./Generate 1 ../Config.txt input.txt function.txt\r\n");
        // fprintf(stderr,"    function.txt will be generated.\r\n");

        // fprintf(stderr,"Test generated function:    ./Generate 1 ../Config.txt input.txt function.txt -t\r\n");
        // fprintf(stderr,"    function.txt will be tested based on input.txt\r\n");
        // // experiment 2, 10000-4, 10000-5, ... , 
        // fprintf(stderr,"experiment 2, specify p = 4/5/.../10 for 10000-4, 10000-5, ..., 10000-10 task.");
        // fprintf(stderr,"Generate function:          ./Generate 2 p input.txt function.txt\r\n");
        // fprintf(stderr,"    function.txt will be generated.\r\n");

        // fprintf(stderr,"Test generated function:    ./Generate 2 p input.txt function.txt -t\r\n");
        // fprintf(stderr,"    function.txt will be tested based on input.txt\r\n");

        // experiment 1, 10000-4
        fprintf(stderr,"experiment 1, 10000-4, different distributions");
        fprintf(stderr,"Generate function:          ./Generate 1 FolderPath\r\n");
        // experiment 2, 10000-4, 10000-5, ... , 
        fprintf(stderr,"experiment 2, iterate 10000-4, 10000-5, ..., 10000-10 tasks.");
        fprintf(stderr,"Generate function:          ./Generate 2 FolderPath\r\n");

        return 1;
    }
    
    ////////////////////////////////////////////////////////////////////////
    // Add more checking logic for bad input args
    ////////////////////////////////////////////////////////////////////////

    int nHash = 10000;
    string path = argv[2];
    listdir(path);
    int fileNum = filePath.size();
    
    // Check experiment and allocate the port[] with port numbers accordingly
    if (strcmp(argv[1],"1")==0)
    {
        // experiment 1: for 108 input distributions
        int pnMax = 4; // each node has pnMax outcomes
        // get number of lines: n
        ifstream fin1("../pn_data/Config.txt");
        char c;
        int n = 0;
        while (fin1.get(c))
        {
            if (c == '\n')
                n++;
        }
        cout << "line count:" << n << endl;
        fin1.close();
        
        int cInterval[n][pnMax];
        // fill the cInterval[][]
        ifstream fin("../pn_data/Config.txt");
        int lineN = 0;
        string line;
        while (getline(fin, line))
        {
            if (line.size() == 0)
            {
                cout << lineN << endl;
                break;
            }
            //cout << "line:" << line << endl;
            istringstream sin(line); //create string input object
            vector<string> Waypoints;
            string info;
            while (getline(sin, info, ' '))
            {
                //cout << "info:" << info << endl;
                Waypoints.push_back(info);
            }

            int sum = 0;
            for (int itr=0; itr<pnMax; itr++)
            {
                string x_str = Waypoints[itr];
                int x;
                stringstream sx;
                sx << x_str;
                sx >> x;
                cInterval[lineN][itr] = x;
                sum += x;
            }
            assert(sum == nHash);
            // cout << lineN <<": " << cInterval[lineN][0] << " " << cInterval[lineN][1]
            //  << " " << cInterval[lineN][2] << " " << cInterval[lineN][3] << endl;
            lineN++;
        }
        fin.close();

        // int nHash = 10000;
        // string path = argv[2];
        int nTrial = n + 1;
        
        ofstream output;
        string outFileName = "../plot/Result_experiment1.txt";
        output.open(outFileName);
        output << fileNum << " experiments. StorageCost in bits, Time in seconds." << endl;
        output << "trial n_a n_b n_c n_d maxStorageCost avgStorageCost maxTime avgTime" << endl;
        
        for (int t=0; t < nTrial; t++)
        {
            int port[nHash];
            int count = 0;
            for (int itv=0; itv<4; itv++)
            {
                int c = cInterval[t][itv];
                while(c > 0)
                {
                    port[count++] = itv+1;
                    c--;
                }
            }
            if(count != nHash)
            {
                fprintf(stderr,"port number not fully allocated. \r\n");
            }
            /////////////////////////////////////////////////////////////////
            // Iterate 1000 input IP sets, and search a best RHF for each IP set.
            // summarize the space consumption.
            fprintf(stderr,"Current trial t: %d \n", t);
            string path = argv[2];
            listdir(path);
            double accTime = 0.0;
            double maxTime = 0.0;
            long accStorageCost = 0;
	        uint64_t maxStorageCost = 0;

            int *pn = port;
            CTimer1 Timer;
            // 1000 experiments, each returns a minimum storage cost, get the global worst case
            for (int i=0;i<fileNum;i++)
            {
                double curTime;
                // double storageCost;
                // fprintf(stderr,"%s\n", filePath[i].c_str());
                const char *curFileP = filePath[i].c_str();
                curTime = Timer.getFileTime(curFileP, pn, maxStorageCost);
                
                accTime += curTime;
                if (curTime > maxTime){
                    maxTime = curTime;
                }
                accStorageCost += Timer.storageCost;
            }
            // After each trial, initialize the storageCost
            Timer.Close();
            // output the max-storage cost out of 1000 exeriments for current trial.
            // fprintf(stderr,"fileNum is: %d\n", fileNum);
            fprintf(stderr,"Total storage cost for all key sets is: %d bit\n", (int)accStorageCost);
            fprintf(stderr,"Maximum storage cost for all key sets is: %d bit\n", (int)(maxStorageCost));
            fprintf(stderr,"avg storage cost for one key set is: %f bit\n", (double)(accStorageCost/fileNum));
            fprintf(stderr,"Total time for all key sets is: %.09f sec\n", accTime);
            fprintf(stderr,"max time for all key set is: %.012f sec\n", maxTime);
            fprintf(stderr,"avg time for one key set is: %.012f sec\n", accTime/fileNum);
            fprintf(stderr,"\n");
            output << t << " " << cInterval[t][0] << " " << cInterval[t][1] << " " << cInterval[t][2] << \
            " " << cInterval[t][3] << " " << int(maxStorageCost) << " " << float(accStorageCost/fileNum) << \
            " " << float(maxTime) << " " << float(accTime/fileNum) << endl;
            /////////////////////////////////////////////////////////////////
        }
    } else if (strcmp(argv[1],"2")==0)
    {
        // experiment 2, different sizes of output interface
        ofstream output;
        string outFileName = "../plot/Result_experiment2.txt";
        output.open(outFileName);
        output << fileNum << " experiments. StorageCost in bits, Time in seconds." << endl;
        output << "output_count maxStorageCost avgStorageCost maxTime avgTime" << endl;
        // output << nBucket << endl;
        // output << nPerSlot << endl;
        // // output each d value in d[](dArrayBest)
        // for (int slot=0;slot<myB.data.Number;slot++)
        // {
        //     output << myB.data[slot] << endl;
        // }
        // For IPs in each interval, we allocate a port number
        int nPort;
        for(nPort=4; nPort<21; nPort++)
        {
            int port[10000];
            int count = 0;
            int interval = (int)(nHash/nPort);
            for (int i=1; i<=nPort; i++)
            {
                int c = interval;
                while(c > 0)
                {
                    port[count++] = i;
                    c--;
                }
            }
            // rest IPs are allocated with IN = nPort
            while (count < nHash) {
                port[count++] = nPort;
            }
            /////////////////////////////////////////////////////////////////
            // Iterate 1000 input IP sets, and search a best RHF for each IP set.
            // summarize the space consumption.
            fprintf(stderr,"Current trial nPort: %d \n", nPort);
            string path = argv[2];
            listdir(path);
            double accTime = 0.0;
            double maxTime = 0.0;
            long accStorageCost = 0;
	        uint64_t maxStorageCost = 0;

            int *pn = port;
            CTimer1 Timer;
            // 1000 experiments, each returns a minimum storage cost, get the global worst case
            for (int i=0;i<fileNum;i++)
            {
                double curTime;
                // double storageCost;
                // fprintf(stderr,"%s\n", filePath[i].c_str());
                const char *curFileP = filePath[i].c_str();
                curTime = Timer.getFileTime(curFileP, pn, maxStorageCost);
                
                accTime += curTime;
                if (curTime > maxTime){
                    maxTime = curTime;
                }
                accStorageCost += Timer.storageCost;
            }
            // After each trial, initialize the storageCost
            Timer.Close();
            // output the max-storage cost out of 1000 exeriments for current trial.
            
            // fprintf(stderr,"fileNum is: %d\n", fileNum);
            fprintf(stderr,"Total storage cost for all key sets is: %d bit\n", (int)accStorageCost);
            fprintf(stderr,"Maximum storage cost for all key sets is: %d bit\n", (int)(maxStorageCost));
            fprintf(stderr,"avg storage cost for one key set is: %f bit\n", (double)(accStorageCost/fileNum));
            fprintf(stderr,"Total time for all key sets is: %.09f sec\n", accTime);
            fprintf(stderr,"max time for all key set is: %.012f sec\n", maxTime);
            fprintf(stderr,"avg time for one key set is: %.012f sec\n", accTime/fileNum);
            fprintf(stderr,"\n");
            output << nPort << " " << int(maxStorageCost) << " " << float(accStorageCost/fileNum)\
             << " " << float(maxTime) << " " << float(accTime/fileNum) << endl;
            /////////////////////////////////////////////////////////////////
        }
    } else
    {
        // wrong experiment number!
        fprintf(stderr,"Please specify the experiment n = 1/2\r\n");
        return 1;
    }

    // if (argc==3) // compare construction time
    // {
    //     string path = argv[???];
    //     listdir(path);

    //     int fileNum = filePath.size();
    //     double accTime = 0.0;
    //     double maxTime = 0.0;
	//     uint64_t maxStorageCost = 0;
    //     for (int i=0;i<fileNum;i++)
    //     {
    //         double curTime;
    //         // fprintf(stderr,"%s\n", filePath[i].c_str());
    //         const char *curFileP = filePath[i].c_str();
    //         if(strcmp(argv[1],"1")==0) {
    //             CTimer1 Timer;
    //             curTime = Timer.getFileTime(curFileP, maxStorageCost);
    //             Timer.Close();
    //         } else if(strcmp(argv[1],"2")==0) {
    //             CTimer2 Timer;
    //             curTime = Timer.getFileTime(curFileP, maxStorageCost);
    //             Timer.Close();
    //         } else if(strcmp(argv[1],"3")==0) {
    //             CTimer3 Timer;
    //             curTime = Timer.getFileTime(curFileP, maxStorageCost);
    //             Timer.Close();
    //         }
            
    //         accTime += curTime;
    //         if (curTime > maxTime){
    //             maxTime = curTime;
    //         }
    //     }
	//     fprintf(stderr,"Maximum storage cost is: %d bit\n", maxStorageCost);
    //     fprintf(stderr,"Total time for all key sets is: %.09f sec\n", accTime);
    //     fprintf(stderr,"avg time for one key set is: %.012f sec\n", accTime/fileNum);
    //     fprintf(stderr,"max time for all key set is: %.012f sec\n", maxTime);
    //     return 0;
    // }

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
