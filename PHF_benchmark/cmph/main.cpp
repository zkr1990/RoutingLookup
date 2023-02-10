#include <unistd.h>
#include <cstdlib>
#include <random>
#include <err.h>
#include "libcmph/include/cmph.h" /* cmph library */
#include <sys/time.h>
// #include <sys/io.h>
// #include <sys/resource.h>
// #include <boost/filesystem.hpp>

#include <dirent.h>
#include <cstring>
#include <vector>
// #include <string>

/*
 * benchmark of two mphf libraries:
 * - phf
 * - emphf
 *
 */
using namespace std;
// Time calculation
float diffUserTime(struct rusage *start, struct rusage *end)
{
    return (end->ru_utime.tv_sec - start->ru_utime.tv_sec) +
           1e-6*(end->ru_utime.tv_usec - start->ru_utime.tv_usec);
}

float diffSystemTime(struct rusage *start, struct rusage *end)
{
    return (end->ru_stime.tv_sec - start->ru_stime.tv_sec) +
           1e-6*(end->ru_stime.tv_usec - start->ru_stime.tv_usec);
}

double do_function(const char* inputFile, const char* algorithm)
{
	//Open file with newline separated list of keys
	FILE * keys_fd = fopen(inputFile, "r");
	cmph_t *hash = NULL;
	if (keys_fd == NULL)
	{
		fprintf(stderr, "File %s not found\n", inputFile);
		exit(1);
	}
	
	struct timeval gStart, gEnd;
    double timeuse = -1;
    gettimeofday(&gStart,NULL);
	
	
	// generate phase
	// Source of keys
	cmph_io_adapter_t *source = cmph_io_nlfile_adapter(keys_fd);
	cmph_config_t *config = cmph_config_new(source);
	string algo = algorithm;
	if (algo == "CMPH_CHM")
	{
		cmph_config_set_algo(config, CMPH_CHM);
	}
	else if (algo == "CMPH_BMZ")
	{
		cmph_config_set_algo(config, CMPH_BMZ);
	}
	// else if (algo == "CMPH_BMZ8") // doesn't work
	// {
	// 	cmph_config_set_algo(config, CMPH_BMZ8);
	// }
	// else if (algo == "CMPH_BRZ") // doesn't work
	// {
	// 	cmph_config_set_algo(config, CMPH_BRZ);
	// }
	// else if (algo == "CMPH_FCH") // very long time
	// {
	// 	cmph_config_set_algo(config, CMPH_FCH);
	// }
	else if (algo == "CMPH_BDZ")
	{
		cmph_config_set_algo(config, CMPH_BDZ);
	}
	else if (algo == "CMPH_BDZ_PH")
	{
		cmph_config_set_algo(config, CMPH_BDZ_PH);
	}
	else if (algo == "CMPH_CHD_PH")
	{
		cmph_config_set_algo(config, CMPH_CHD_PH);
	}
	else if (algo == "CMPH_CHD")
	{
		cmph_config_set_algo(config, CMPH_CHD);
	}
	else
	{	
		perror("Check input algorithm name again.");
		throw "Check input algorithm name again.";
	}
	hash = cmph_new(config);
	cmph_config_destroy(config);
	
	gettimeofday(&gEnd,NULL);
	timeuse = gEnd.tv_sec - gStart.tv_sec + (gEnd.tv_usec - gStart.tv_usec)/1000000.0;
	
	// fprintf(stderr,"Use Time:%f\r\n",timeuse);
	
	//dump mphf parameters into a file
	FILE * mphf_fd = fopen("function_file", "w");
	cmph_dump(hash, mphf_fd);
	fclose(mphf_fd);

	// query phase
	FILE * f = fopen("function_file", "r");
	fseek(f, 0, SEEK_END);
	unsigned long len = (unsigned long)ftell(f);
	/* add query logic here:
	// const char *key1 = "2432620340";
	// unsigned int id1 = cmph_search(hash, key1, (cmph_uint32)strlen(key1));
  	// fprintf(stderr, "Id:%u\n", id1);
	*/
	fclose(f);
	
	//Destroy hash
	cmph_destroy(hash);
	cmph_io_nlfile_adapter_destroy(source);
	fclose(keys_fd);
	return timeuse;
}

vector<string> filePath;

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

int main (int argc, char* argv[])
{
	string path = argv[1];

	listdir(path);

	int fileNum = filePath.size();
	double accTime = 0.0;
	double maxTime = 0.0;
	const char *algo = argv[2];
	for (int i=0;i<fileNum;i++)
	{
		fprintf(stderr,"%s\n", filePath[i].c_str());
		const char *curFileP = filePath[i].c_str();
		double curTime = do_function(curFileP, algo);
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
