/*
 * benchmarkUtils.h
 *  All benchmark related supporting functions.
 *  Shared across entire assignment 1.
 *  Created on: Sep 27, 2015
 */

#ifndef BENCHMARKUTILS_H_
#define BENCHMARKUTILS_H_

#include <stdint.h>
#include <sys/time.h>  // to get time for unique filename creation.
#include <time.h>      // for clock functions.
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <stdlib.h>    // for quicksort for quartile values calculation for report.

#define BILLION 1000000000L
#define MAX_RUNS 1000

using namespace std;

uint64_t monotonicClk[MAX_RUNS] = {0};
uint64_t processCpuTime[MAX_RUNS] = {0};

uint64_t diffTime(struct timespec start, struct timespec end);
int createCSVReport(char *pBenchmarkTitle, int maxRuns, vector<string> *pStrMsg);



int compare (const void * a, const void * b)
{
  return ( *(uint64_t*)a - *(uint64_t*)b );
}

int createCSVReport(char *pBenchmarkTitle, int maxRuns, vector<string> *pStrMsg)
{
    time_t rawtime;
    struct tm* timeinfo;
    char timeYyMMdd [80];
    char filename[200];
    int run = 0;
    FILE *fp;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    printf ("Creating CSV report");

    strftime(timeYyMMdd,80,"%Y-%m-%d-%H-%M-%S",timeinfo);
    puts(timeYyMMdd);

    sprintf(filename,"%s_Benchmark_%s.csv", timeYyMMdd, pBenchmarkTitle);

    fp = fopen(filename, "w");

    fprintf(fp, "Benchmark Name,%s,\n",pBenchmarkTitle);
    fprintf(fp, "Filename,%s,\n",filename);
    fprintf(fp, "Time,%s,\n\n\n",timeYyMMdd);
    fprintf(fp, "Experiment Run, ,CLOCK_MONOTONIC, Unit,CLOCK_PROCESS_CPUTIME_ID, Unit,\n");

    printf("Benchmark Name,%s,\n",pBenchmarkTitle);
    printf("Filename,%s,\n",filename);
    printf("Time,%s,\n\n\n",timeYyMMdd);
    printf("Experiment Run, ,CLOCK_MONOTONIC, Unit,CLOCK_PROCESS_CPUTIME_ID, Unit,\n");

    for (run = 0; run < maxRuns; run++)
    {
        fprintf(fp, "%d,%llu,%llu,\n",
                run,
                (long long unsigned int) monotonicClk[run],
                (long long unsigned int) processCpuTime[run]);

        printf("%03d Actual) %s, (Elapsed time, Elapsed process CPU time) = (,%llu, nanoseconds,%llu, nanoseconds)\n",
                run,
                pBenchmarkTitle,
                (long long unsigned int) monotonicClk[run],
                (long long unsigned int) processCpuTime[run]);

        if (!!pStrMsg &&
                (int)pStrMsg->size() > run)
        {
            fprintf(fp, "%s", pStrMsg->at(run).c_str());
            printf( "%s", pStrMsg->at(run).c_str());

        }
        fflush(stdout);
    }

    fprintf(fp,"\n\n\n\n");
    printf("\n\n\n\n");

    fprintf(fp,"SORTED Result\n");  
    fprintf(fp, "Experiment Run, ,CLOCK_MONOTONIC, Unit,CLOCK_PROCESS_CPUTIME_ID, Unit,\n");

    printf("SORTED Result\n");
    printf("Experiment Run, ,CLOCK_MONOTONIC, Unit,CLOCK_PROCESS_CPUTIME_ID, Unit,\n");

    qsort(monotonicClk, maxRuns, (size_t)sizeof(uint64_t), compare);
    qsort(processCpuTime, maxRuns, (size_t)sizeof(uint64_t), compare);

    for (run = 0; run < maxRuns; run++)
    {
        fprintf(fp, "%d Sorted,%llu,%llu,\n",
                        run,
                        (long long unsigned int) monotonicClk[run],
                        (long long unsigned int) processCpuTime[run]);

        printf("%03d Sorted) %s, (Elapsed time, Elapsed process CPU time) = (,%llu, nanoseconds,%llu, nanoseconds)\n",
                        run,
                        pBenchmarkTitle,
                        (long long unsigned int) monotonicClk[run],
                        (long long unsigned int) processCpuTime[run]);
    }

    const int Q1 = maxRuns / 4; // Q1
    const int Q2 = maxRuns / 2; // median
    const int Q3 = Q1 + Q2;      // Q3

    fprintf(fp, "\n\n\nMin,%llu,%llu,\n",
                    (long long unsigned int) monotonicClk[0],
                    (long long unsigned int) processCpuTime[0]);
    fprintf(fp, "Q1,%llu,%llu,\n",
                        (long long unsigned int) monotonicClk[Q1],
                        (long long unsigned int) processCpuTime[Q1]);
    fprintf(fp, "Median,%llu,%llu,\n",
                        (long long unsigned int) monotonicClk[Q2],
                        (long long unsigned int) processCpuTime[Q2]);
    fprintf(fp, "Q3,%llu,%llu,\n",
                        (long long unsigned int) monotonicClk[Q3],
                        (long long unsigned int) processCpuTime[Q3]);
    fprintf(fp, "Max,%llu,%llu,\n",
                            (long long unsigned int) monotonicClk[maxRuns -1],
                            (long long unsigned int) processCpuTime[maxRuns -1]);

    printf("\n\n\nMin,%llu,%llu,\n",
                    (long long unsigned int) monotonicClk[0],
                    (long long unsigned int) processCpuTime[0]);
    printf("Q1,%llu,%llu,\n",
                        (long long unsigned int) monotonicClk[Q1],
                        (long long unsigned int) processCpuTime[Q1]);
    printf("Median,%llu,%llu,\n",
                        (long long unsigned int) monotonicClk[Q2],
                        (long long unsigned int) processCpuTime[Q2]);
    printf("Q3,%llu,%llu,\n",
                        (long long unsigned int) monotonicClk[Q3],
                        (long long unsigned int) processCpuTime[Q3]);
    printf("Max,%llu,%llu,\n",
                            (long long unsigned int) monotonicClk[maxRuns -1],
                            (long long unsigned int) processCpuTime[maxRuns -1]);

    return 0;
}

uint64_t diffTime(struct timespec start, struct timespec end)
{
    uint64_t diff = 0;
    diff = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;

    return diff;
}



#endif /* BENCHMARKUTILS_H_ */
