/***************************************************************************
*	FILE: processFunctions.c                                               *  
*	AUTHOR: Samantha Taylor                                                *
*   STUDENT NUMBER: 18863152                                               *
*	UNIT: COMP2006                                                         *
*	PURPOSE: Implementing the reader and writer functions                  *
*   LAST MOD: 30/05/18                                                     *
***************************************************************************/
#include "processFunctions.h"

/* **************************************************************************** *
 * Import: FileData is in the file contents in an array                         *
 *         Lock contains the semaphore and count to mutually access shared data *
 *         ProcessInfo is an array of process info used as a buffer for sim_out *
 *         Buffer is the pointer to the shared memory array                     *
 * Purpose: Write to the shared data buffer. Only one writer can access the     *
 *         buffer at a time.                                                    *
 * **************************************************************************** */
void writer(int fileLength, int* buffer, Locks* lock, int timeout, ProcessInfo* signoutBuffer, int* fileData)
{
  /* CREATES PROCESS INFO STRUCT */    
  ProcessInfo* proInfo = (ProcessInfo*)malloc(sizeof(ProcessInfo));
  proInfo->counter = 0;
  proInfo->childPID = getpid();
  proInfo->processType = 'w';

  do
  {
    /* ACQUIRE RW LOCK */
    sem_wait(&(lock->rw_mutex));
      /* WRITE DATA FROM FILE TO BUFFER */
      buffer[ (proInfo->counter) % BUFFER_SIZE ] = fileData[proInfo->counter];
    /* RELEASE RW LOCK */
    sem_post(&(lock->rw_mutex));
    proInfo->counter++;

    /* WRITER HAS WRITTEN ALL THE FILE DATA */
    if( (proInfo->counter) == fileLength)
    {
      int index = 0;
      /* ACQUIRE OUT BUFFER LOCK */
      sem_wait(&(lock->outFileMutex));
        index = lock->numProcessesTerminate;

        /* ADDS PROCESS INFORMATION TO OUT BUFFER */
        signoutBuffer[index] = *proInfo;
        lock->numProcessesTerminate++;
      /* RELEASE OUT BUFFER LOCK */
      sem_post(&(lock->outFileMutex));

      exit(0);
    }
    else
    {
      sleep(timeout);
    }
  }while(proInfo->counter != (fileLength));
}

/* **************************************************************************** *
 * Import: Lock contains the semaphore and count to mutually access shared data *
 *         ProcessInfo is an array of process info used as a buffer for sim_out *
 *         Buffer is the pointer to the shared memory array                     *
 * Purpose: Read the shared data buffer. Many readers can read at the same time *
 * **************************************************************************** */
void reader(int fileLength, int* buffer, Locks* lock, int timeout, ProcessInfo* signoutBuffer)
{
  /* CREATES PROCESS INFO STRUCT */
  ProcessInfo* proInfo = (ProcessInfo*)malloc(sizeof(ProcessInfo));
  proInfo->counter = 0;
  proInfo->childPID = getpid();
  proInfo->processType = 'r';

  int* intsRead = (int*)calloc(fileLength, sizeof(int));
  int result = 0;
  do
  {
    /* ACQUIRE COUNT LOCK */
    sem_wait(&(lock->countMutex));
      lock->numReaders++;
      if(lock->numReaders == 1)
      {
        /* ACQUIRE RW LOCK */
        sem_wait(&(lock->rw_mutex));

      }
    /* RELEASE COUNT LOCK */
    sem_post(&(lock->countMutex));
    
    /* PERFORM READING */
    result = compare(intsRead, fileLength, buffer[ (proInfo->counter) % BUFFER_SIZE ]);

    /* IF NEW NUMBER READ ISNT IN THE ARRAY -> ADD IT */
    if( result != FOUND)
    {
        /* COPY BUFFER ELEMENTS INTO ARRAY */
        intsRead[proInfo->counter] = buffer[ (proInfo->counter) % BUFFER_SIZE ];
    }

    /* OTHERWISE OFFSET THE COUNTER */
    else
    {
        proInfo->counter--;
    }
    /* ACQUIRE COUNT LOCK */
    sem_wait(&(lock->countMutex));
      lock->numReaders--;

      /* IF READERS HAVE FINISHED SIGNAL ANY WAITING WRITERS*/
      if(lock->numReaders == 0)
      {
          /* RELEASE RW LOCK */
          sem_post(&(lock->rw_mutex));
      }
    /* RELEASE COUNT LOCK */
    sem_post(&(lock->countMutex));

    proInfo->counter++;

    /* READER HAS READ ALL THE FILE DATA */
    if( (proInfo->counter) == fileLength)
    {
      int index = 0;

      /* ACQUIRE FILE OUT BUFFER LOCK */
      sem_wait(&(lock->outFileMutex));
        index = lock->numProcessesTerminate;
        signoutBuffer[index] = *proInfo;
        lock->numProcessesTerminate++;
      /* RELEASE FILE OUT BUFFER LOCK */
      sem_post(&(lock->outFileMutex));

      /* CLEAN UP */
      free(intsRead);
      exit(0);
    }
    else
    {
      sleep(timeout);
    }
  }while(proInfo->counter != (fileLength));
}

/* Import: Array of intergers, and a number
 * Export: if number was found in the array 
 * Purpose: Binary search for the newNUm */
int compare(int* array, int arrayLength, int newNum)
{
    int ii = 0;
    int result = 0;

    for(ii = 0; ii < arrayLength; ii++)
    {
        if(array[ii] == newNum)
        {
            result = FOUND;
        }
    }

    return result;
}
