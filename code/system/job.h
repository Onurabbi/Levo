#ifndef JOB_H_
#define JOB_H_


#define MAX_NUM_JOBS    8

bool initJobs(void);
void performJobs(void (*JobFunction)(void *));
void finishThreadJob(void);

#endif