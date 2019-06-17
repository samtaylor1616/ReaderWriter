/***************************************************************************
*	FILE: processFunctions.h                                               *
*	AUTHOR: Samantha Taylor                                                *
*   STUDENT NUMBER: 18863152                                               *
*	UNIT: COMP2006                                                         *
*	PURPOSE: Implementing the reader and writer functions                  *
*   LAST MOD: 3/05/18                                                      *
***************************************************************************/
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include <errno.h>
#include <string.h>

#ifndef BUFFER_SIZE
    #define BUFFER_SIZE 20
#endif

#define FOUND 1


/* ********************************************************* *
 * Any information on the structs listed below can be found  *
 * in main.h                                                 *
 * ********************************************************* */
#ifndef LOCK_GUARD
#define LOCK_GUARD
  typedef struct{
    pthread_mutex_t rw_mutex;
    pthread_mutex_t countMutex;
    pthread_mutex_t outFileMutex;

    pthread_cond_t readersFinished;
    pthread_cond_t writerFinished;

    int numThreadsTerminate;
    int numReaders;
    int numWriters;
  }Locks;
#endif

#ifndef THREAD_INFO_GUARD
#define THREAD_INFO_GUARD
  typedef struct{
    int counter;
    int TID;
    char threadType;
  }ThreadInfo;
#endif

#ifndef WRITER_STRUCT
#define WRITER_STRUCT
  typedef struct{
    int fileLength;
    int* buffer;
    Locks* lock;
    int timeout;
    ThreadInfo* signoutBuffer;
    int* fileData;
    int TID;
  }WriterStruct;
#endif

#ifndef READER_STRUCT
#define READER_STRUCT
  typedef struct{
    int fileLength;
    int* buffer;
    Locks* lock;
    int timeout;
    ThreadInfo* signoutBuffer;
    int TID;
  }ReaderStruct;
#endif

/* PROTOTYPES */
void* writer_runner(void*);
void* reader_runner(void*);
int compare(int*, int, int);
