#include <iostream>
#include <time.h>       /* timer */

#define GRPC_PROFILE_DEBUG_MODE     1

#include <stdint.h>

//  Windows
#ifdef _WIN32

	#include <intrin.h>
	uint64_t rdtsc(){
		return __rdtsc();
	}

//  Linux/GCC
#else

	uint64_t rdtsc(){
		unsigned int lo,hi;
		__asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
		return ((uint64_t)hi << 32) | lo;
	}

#endif


struct FuncStats
{
    std::string funcName;
    std::string fileName;
    std::string description;
    uint64_t start_ns;
    uint64_t end_ns;
    uint64_t duration_ns;

    FuncStats()
    {
    }

    FuncStats(std::string func_name, uint64_t start_nsec, uint64_t end_nsec, std::string file_name = "", std::string desc = "")
    {
        funcName = func_name;
        start_ns = start_nsec;
        end_ns = end_nsec;
        duration_ns = end_nsec - start_nsec;
        fileName = (file_name.length() > 0) ? file_name : " ";
        description = (desc.length() > 0) ? desc : " ";
    }
};

std::vector<FuncStats> funcProfiler;

int add_func_stats(std::string func_name, uint64_t start_ns, uint64_t end_nsec, std::string file_name, std::string desc)
{
    FuncStats currFuncStat(func_name, start_ns, end_nsec, file_name, desc);

    if (GRPC_PROFILE_DEBUG_MODE)
    {
        std::cout << func_name												\
        	<< ", Start:" << currFuncStat.start_ns << " ns,"  				\
			<< ", End:" << currFuncStat.end_ns << " ns,"  				\
            << " Duration:" << currFuncStat.duration_ns << " ns,"          \
            << " Description:" << currFuncStat.description << ","          \
            << " FileName:" << currFuncStat.fileName << std::endl;
    }
    funcProfiler.push_back(currFuncStat);
    return 0;
}

int print_to_file()
{
	FILE *fp;
	std::string resultFileName = "cs739_grpc_result.csv";
	fp = fopen(resultFileName.c_str(), "a");

    FuncStats this_stat;
    for (unsigned int i = 0; i <  (unsigned int)funcProfiler.size(); i++)
    {
        this_stat = funcProfiler[i];
        fprintf(fp, "%s,%ld,%ld\n", this_stat.funcName.c_str(), this_stat.start_ns, this_stat.duration_ns);
    }

	fclose(fp);
}

void print_all_profile_stats()
{
    FuncStats this_stat;
    for (unsigned int i = 0; i <  (unsigned int)funcProfiler.size(); i++)
    {
        this_stat = funcProfiler[i];
        printf("%s,%ld,%ld\n", this_stat.funcName.c_str(), this_stat.start_ns, this_stat.duration_ns);
    }

    print_to_file();
}


//to get the time
uint64_t nanos_since_midnight()
{
    return rdtsc();//std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}
