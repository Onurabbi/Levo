#ifndef MEMORY_H_
#define MEMORY_H_

#define KILOBYTES(num) ((num) * (1024ULL))
#define MEGABYTES(num) ((KILOBYTES(num)) * (1024ULL))
#define GIGABYTES(num) ((MEGABYTES(num)) * (1024ULL))

#define GAME_MEMORY_SIZE GIGABYTES(1)
#define PERMANENT_MEMORY_SIZE MEGABYTES(512)
#define TRANSIENT_MEMORY_SIZE (GAME_MEMORY_SIZE - PERMANENT_MEMORY_SIZE)

bool   initMemory(void);
void * allocatePermanentMemory(size_t size);

void   resetTransientMemory(void);
void * allocateTransientMemory(size_t size);
#endif