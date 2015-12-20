#include <time.h>       /* timer */

#define GRPC_PROFILE_DEBUG_MODE     1

#include <stdint.h>
#include <string.h>

/*  Windows */
#ifdef _WIN32

	#include <intrin.h>
	uint64_t rdtsc_c(){
		return __rdtsc();
	}

/*  Linux/GCC */
#else

	uint64_t rdtsc_c(){
		unsigned int lo,hi;
		__asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
		return ((uint64_t)hi << 32) | lo;
	}

#endif


struct FuncStats_c
{
    char funcName[255];
    char fileName[255];
    char description[512];
    uint64_t start_ns;
    uint64_t end_ns;
    uint64_t duration_ns;
};

void FuncStats_c_init(struct FuncStats_c *this, char *func_name, uint64_t start_nsec, uint64_t end_nsec, char* file_name, char* desc)
{
    strcpy(this->funcName, func_name);
    this->start_ns = start_nsec;
    this->end_ns = end_nsec;
    this->duration_ns = end_nsec - start_nsec;
    strcpy(this->fileName, file_name); /*.length() > 0) ? file_name : " ";*/
    strcpy(this->description, desc); /* > 0) ? desc : " ";*/
}

struct FuncStats_c funcProfiler[255];
long counter = 0;

int add_func_stats_c(char * func_name, uint64_t start_ns, uint64_t end_nsec, char * file_name, char * desc)
{
    struct FuncStats_c currFuncStat;

    FuncStats_c_init(&currFuncStat, func_name, start_ns, end_nsec, file_name, desc);

    if (GRPC_PROFILE_DEBUG_MODE)
    {
        printf("%s,%ld,%ld,%s\n\n", func_name,
        	currFuncStat.start_ns,
            currFuncStat.duration_ns,
            currFuncStat.description);
            
    }
    funcProfiler[counter++] = currFuncStat;
    return 0;
}

int print_to_file_c()
{
	static FILE *fp;
	static char resultFileName[] = "cs739_grpc_result.csv";
	struct FuncStats_c this_stat;
    unsigned int i = 0;

    fp = fopen(resultFileName, "a");
    
    for (i = 0; i <  (unsigned int)counter; i++)
    {
        this_stat = funcProfiler[i];
        fprintf(fp, "%s,%ld,%ld\n", this_stat.funcName, this_stat.start_ns, this_stat.duration_ns);
    }

	fclose(fp);
    return 0;
}

void print_all_profile_stats_c()
{
    struct FuncStats_c this_stat;
    unsigned int i;
    for (i = 0; i <  (unsigned int)counter; i++)
    {
        this_stat = funcProfiler[i];
        printf("%s,%ld,%ld\n", this_stat.funcName, this_stat.start_ns, this_stat.duration_ns);
    }

    print_to_file_c();
}


/*to get the time*/
uint64_t nanos_since_midnight_c()
{
    return rdtsc_c();
}
