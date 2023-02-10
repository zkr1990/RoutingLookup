#include <stdio.h>
#include <cstring>
#include <math.h>
#include <unistd.h>
#include <ios>
#include <vector>
#include <dirent.h>
#include <sys/io.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>

//------------------------------------------------------------------------------
using namespace std;

//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int nHash = 10000;
    int pnMax = 4; // each node has pnMax outcomes
    // get number of lines: n
    ifstream fin1("Config0.txt");
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
    ifstream fin("Config0.txt");
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
    
    for (int t=0; t < nTrial; t++)
    {
        int port[nHash];
        int count = 0;
        for (int itv=0; itv<pnMax; itv++)
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
    }
    
    return 0;
}
//------------------------------------------------------------------------------
