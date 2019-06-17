/***************************************************************************
*	FILE: main.h
*	AUTHOR: Samantha Taylor
* STUDENT NUMBER: 18863152
*	UNIT: COMP2006
*	PURPOSE: Implementing Reader/Writer problem using processes
*   LAST MOD: 30/05/18
***************************************************************************/
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <sys/shm.h>
#include <errno.h>
#include <string.h>

#include "FileIO.h"
#include "processFunctions.h"

#include <stdlib.h>

#ifndef BUFFER_SIZE
    #define BUFFER_SIZE 20
#endif

#define FILENAME "shared_data.txt"
#define OUTFILE "sim_out.txt"


/* *********************************************************************************** *
 * Mutex_rw: is a binary semaphore, for the first or last readers and writer processes *
 * Count mutex: is a binary semaphore that ensures mutual exclusion for when the       *
 *              read_count is updated                                                  *
 * OutFileMutex: is a binary semaphore, for processes accessing the buffer for signing *
 *              out processes                                                          *
 * NumReaders: is an integer count of the number of reader processes currently         *
 *              accessing the buffer                                                   *
 * NumProcessesTerminate: is an integer count of the total number of processes that    *
 *              have finished their task and have terminated                           *
 *                                                                                     *
 * Each of the mutexes are initially set to 1                                          *
 * NumReaders is initially set to 0                                                    *
 * The Mutexes provide mutual exclusion to the shared variables (numReaders,           *
 *              numProcessesTerminated, any buffers)                                   *
 * *********************************************************************************** */
#ifndef LOCK_GUARD
#define LOCK_GUARD
  typedef struct{
    sem_t rw_mutex;
    sem_t countMutex;
    sem_t outFileMutex;
    int numProcessesTerminate;
    int numReaders;
  }Locks;
#endif

/* **************************************************************************************** *
 * Counter: is the number of unique characters that have been read or writen by the process *
 * childPID: is the process ID number of the process                                        *
 * processType: is a char, establishing the type of the process ('r' for reader and 'w' for *
 *              writer)                                                                     *
 * This struct is used to contain any information of a process                              *
 * **************************************************************************************** */
#ifndef PROCESS_INFO_GUARD
#define PROCESS_INFO_GUARD
  typedef struct{
    int counter;
    int childPID;
    char processType;
  }ProcessInfo;
#endif


/* PROTOTYPES */
int main(int, char**);
int startProcess(int, int, int, int);
int createLocks(Locks*);
int deleteLocks(Locks*);
