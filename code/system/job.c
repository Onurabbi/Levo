#include "../common.h"

#include "job.h"

static Job           jobQueue[MAX_NUM_JOBS];
static uint32_t      readOffset;
static uint32_t      writeOffset;
static uint32_t      jobCount;

static SDL_Thread  * threads[MAX_NUM_THREADS];

SDL_mutex * queueMutex;
SDL_cond *  queueCond;

SDL_mutex  * finishMutex;
SDL_cond * finishCondition;

static int numFinishedThreads;
static int numThreads[MAX_NUM_THREADS];
static Job jobs[MAX_NUM_THREADS];


static void startThreadJobs(void)
{
    SDL_LockMutex(finishMutex);
    numFinishedThreads = 0;
    SDL_UnlockMutex(finishMutex);
}

void finishThreadJob(void)
{
    SDL_LockMutex(finishMutex);
    numFinishedThreads++;
    SDL_UnlockMutex(finishMutex);
    if(numFinishedThreads == MAX_NUM_THREADS)
    {
        SDL_CondSignal(finishCondition);
    }
}

static void waitForThreadsToFinish(void)
{
    SDL_LockMutex(finishMutex);
    {
        while(numFinishedThreads < MAX_NUM_THREADS)
        {
            SDL_CondWait(finishCondition, finishMutex);
        }
    }
    SDL_UnlockMutex(finishMutex);
}

static void executeJob(Job * job)
{
    job->JobFunction(job->context);
}

static int threadFunction(void * data)
{
    for(;;)
    {
        Job job;

        SDL_LockMutex(queueMutex);
        while(jobCount == 0)
        {
            SDL_CondWait(queueCond, queueMutex);
        }
        job = jobQueue[readOffset];
        readOffset = (readOffset + 1) % MAX_NUM_JOBS;
        jobCount--;
        SDL_UnlockMutex(queueMutex);
        executeJob(&job);
    }
}

static void submitJob(Job job)
{
    SDL_LockMutex(queueMutex);
    jobQueue[writeOffset] = job;
    writeOffset = (writeOffset + 1) % MAX_NUM_JOBS;
    jobCount++;
    SDL_UnlockMutex(queueMutex);
    SDL_CondSignal(queueCond);
}

void performJobs(void (*JobFunction)(void *))
{
    startThreadJobs();

    for(int i = 0; i < MAX_NUM_THREADS; i++)
    {
        numThreads[i] = i;
        jobs[i].context = &numThreads[i];
        jobs[i].JobFunction = JobFunction;
        submitJob(jobs[i]);
    }

    waitForThreadsToFinish();
}

bool initJobs(void)
{
    queueMutex = SDL_CreateMutex();
    if(queueMutex == NULL)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_ERROR, 
                       "Can't create SDL mutex! Error: %s\n", SDL_GetError());
        return false;
    }

    queueCond = SDL_CreateCond();
    if(queueCond == NULL)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_ERROR, 
                       "Can't create SDL condition! Error: %s\n", SDL_GetError());
        return false;
    }
    
    finishMutex = SDL_CreateMutex();
    if(finishMutex == NULL)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_ERROR, 
                       "Can't create SDL mutex! Error: %s\n", SDL_GetError());
        return false;
    }

    finishCondition = SDL_CreateCond();
    if(finishCondition == NULL)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_ERROR, 
                      "Can't create SDL condition! Error: %s\n", SDL_GetError());
        return false; 
    }

    for(int i = 0; i < MAX_NUM_THREADS; i++)
    {
        char Buf[MAX_NAME_LENGTH];
        sprintf(Buf, "Thread%d", i);
        threads[i] = SDL_CreateThread(threadFunction, Buf, NULL);
        if(threads[i] == NULL)
        {
            SDL_LogMessage(SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_ERROR, 
                       "Can't create SDL thread %d! Error: %s\n", i, SDL_GetError());
            return false;
        }
    }

    jobCount = 0;
    writeOffset = 0;
    readOffset = 0;
    numFinishedThreads = 0;

    return true;
}

