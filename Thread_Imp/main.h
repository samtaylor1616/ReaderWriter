/***************************************************************************
*	FILE: main.h
*	AUTHOR: Samantha Taylor
*   STUDENT NUMBER: 18863152
*	UNIT: COMP2006
*	PURPOSE: Implementing Reader/Writer problem using threads
*   LAST MOD: 3/05/18
************************************************************************** */
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <errno.h>
#include <string.h>

#include "FileIO.h"
#include "threadFunctions.h"


#ifndef BUFFER_SIZE
    #define BUFFER_SIZE 20
#endif

#define FILENAME "shared_data.txt"
#define OUTFILE "sim_out.txt"


/* ************************************************************************************** *
 * Mutex_rw: is a pthread mutex(lock), for the first or last readers and writer threads   *
 * Count mutex: is a pthread mutex(lock) that ensures mutual exclusion for when the       *
 *              read_count is updated                                                     *
 * OutFileMutex: is a pthread mutex(lock), for processes accessing the buffer for signing *
 *              out threads                                                               *
 * NumReaders: is an integer count of the number of reader threads currently accessing    *
 *              the buffer                                                                *
 * NumWriters: is an integer count to determine if a writer thread is writing (0 or 1)    *
 * NumThreadsTerminate: is an integer count of the total number of threads that have      *
 *              finished their task and have terminated                                   *
 *                                                                                        *
 * The Mutexes provide mutual exclusion to the shared variables (numReaders,              *
 *              numThreadsTerminated, any buffers)                                        *
 * ************************************************************************************* */
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

/* **************************************************************************************** *
 * Counter: is the number of unique characters that have been read or writen by the thread  *
 * TID: is the thread ID number                                                             *
 * processType: is a char, establishing the type of the thread  ('r' for reader and 'w' for *
 *              writer)                                                                     *
 * This struct is used to contain any information of a thread                               *
 * **************************************************************************************** */
#ifndef THREAD_INFO_GUARD
#define THREAD_INFO_GUARD
  typedef struct{
    int counter;
    int TID;
    char threadType;
  }ThreadInfo;
#endif

/* **************************************************************************************** *
 * Stuct contains any information a writer thread needs to perform the writing function     *
 *    Buffer is the data buffer shared between the reader and writer threads                *
 *    Lock contains the pthread mutexes that provide mutural exclusion                      *
 *    Timeout is the respective time the thread should wait (provided by the user)          *
 *    SignoutBuffer is an array shared between all the threads for when they have finished  *
 *        their task. The main threads handles this buffer before exiting                   *
 *    Filedata is an array of the file contents providing easy access for writer threads    *
 *    TID is the thread ID number used to uniquely identify the thread                      *
 * **************************************************************************************** */
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

/* **************************************************************************************** *
 * Stuct contains any information a reader thread needs to perform the reading function     *
 * Variables of struct are described above in the WriterStruct                              *
 * **************************************************************************************** */
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
int main(int, char**);
int startThreads(int, int, int, int);
WriterStruct* setUpWriter(int, int*, Locks*, int, ThreadInfo*, int*, int);
ReaderStruct* setUpReader(int, int*, Locks*, int, ThreadInfo*, int);
int createLocks(Locks*);
int deleteLocks(Locks*);
