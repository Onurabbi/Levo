#ifndef JOB_H_
#define JOB_H_


#define MAX_NUM_JOBS    MAX_NUM_THREADS

bool initJobs(void);
void performJobs(void (*JobFunction)(void *));
void finishThreadJob(void);

#endif