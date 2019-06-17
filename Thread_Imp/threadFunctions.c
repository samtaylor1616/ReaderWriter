/***************************************************************************
*	FILE: processFunctions.c                                               *
*	AUTHOR: Samantha Taylor                                                *
*   STUDENT NUMBER: 18863152                                               *
*	UNIT: COMP2006                                                         *
*	PURPOSE: Implementing the reader and writer functions                  *
*   LAST MOD: 30/05/18                                                     *
***************************************************************************/
#include "threadFunctions.h"

/* **************************************************************************** *
 * Import: WriterStruct pointer cast as a void pointer                          *
 * Export: Nothing                                                              *
 * Purpose: Write to the shared data buffer. Only one writer can access the     *
 *         buffer at a time.                                                    *
 * **************************************************************************** */
void* writer_runner(void* arg)
{
  /* Copy information out of argument passed in */
  WriterStruct* writer = (WriterStruct*)arg;
  int fileLength = writer->fileLength;
  int* buffer = writer->buffer;
  Locks* lock = writer->lock;
  int timeout = writer->timeout;
  ThreadInfo* signoutBuffer = writer->signoutBuffer;
  int* fileData = writer-> fileData;

  /* CREATES THREAD INFO STRUCT */
  ThreadInfo* threadInfo = (ThreadInfo*)malloc(sizeof(ThreadInfo));
  threadInfo->counter = 0;
  threadInfo->TID = writer->TID;
  threadInfo->threadType = 'w';

  /* CLEAN UP PASSED IN ARGUMENTS */
  free(writer);
  do
  {
    /* ACQUIRE COUNT LOCK */
    pthread_mutex_lock(&(lock->countMutex));
      while(lock->numWriters == 1)
      {
          /* WAIT FOR WRITER TO FINISH WRITING */
          pthread_cond_wait(&(lock->writerFinished), &(lock->countMutex));
      }
      while(lock->numReaders > 0)
      {
        /* WAIT FOR READERS TO FINISH READING */
        pthread_cond_wait(&(lock->readersFinished), &(lock->countMutex));
      }
      lock->numWriters++;
    /* ACQUIRE RW LOCK */
    pthread_mutex_lock(&(lock->rw_mutex));

    /* WRITE DATA FROM FILE TO BUFFER */
    buffer[ (threadInfo->counter) % BUFFER_SIZE ] = fileData[threadInfo->counter];
    
    lock->numWriters--;
    pthread_mutex_unlock(&(lock->countMutex));
    /* SIGNAL ALL THREADS WAITING ON WRITER TO FINISH */
    pthread_cond_broadcast(&(lock->writerFinished));
    /* RELEASE RW LOCK */
    pthread_mutex_unlock(&(lock->rw_mutex));

    threadInfo->counter++;

    /* WRITER HAS WRITTEN ALL THE FILE DATA */
    if( (threadInfo->counter) == fileLength)
    {
      int index = 0;
     /* ACQUIRE FILE COUNT MUTEX LOCK */ 
      pthread_mutex_lock(&(lock->countMutex));
      /* ACQUIRE FILE OUT BUFFER LOCK */
      pthread_mutex_lock(&(lock->outFileMutex));
        index = lock->numThreadsTerminate;
        signoutBuffer[index] = *threadInfo;
        lock->numThreadsTerminate++;
      /* RELEASE FILE OUT BUFFER LOCK */
      pthread_mutex_unlock(&(lock->outFileMutex));
      /* RELEASE COUNT LOCK */
      pthread_mutex_unlock(&(lock->countMutex));

      pthread_exit(NULL);
    }
    else
    {
      sleep(timeout);
    }
  }while(threadInfo->counter != (fileLength));
  return 0;
}

/* **************************************************************************** *
 * Import: ReaderStruct pointer cast as a void pointer                          *
 * Purpose: Read the shared data buffer. Many readers can read at the same time *
 * **************************************************************************** */
void* reader_runner(void* arg)
{
  /* Copy information out of argument passed in */
  ReaderStruct* r = (ReaderStruct*)arg;
  int fileLength = r->fileLength;
  int* buffer = r->buffer;
  Locks* lock = r->lock;
  int timeout = r->timeout;
  ThreadInfo* signoutBuffer = r->signoutBuffer;

  /* CREATES THREAD INFO STRUCT */
  ThreadInfo* threadInfo = (ThreadInfo*)malloc(sizeof(ThreadInfo));
  threadInfo->counter = 0;
  threadInfo->TID = r->TID;
  threadInfo->threadType = 'r';

  /* CLEAN UP PASSED IN ARGUMENT */
  free(r);

  int* intsRead = (int*)calloc(fileLength, sizeof(int));
  int result = 0;
  do
  {
    /* ACQUIRE COUNT LOCK */
    pthread_mutex_lock(&(lock->countMutex));
      while(lock->numWriters == 1)
      {
        /* WAIT FOR WRITER TO FINISH WRITING */
        pthread_cond_wait(&(lock->writerFinished), &(lock->countMutex));
      }
      lock->numReaders++;
      if(lock->numReaders == 1)
      {
        /* FIRST READER ACQUIRES RW LOCK */
        pthread_mutex_lock(&(lock->rw_mutex));
      }
    /* RELEASE COUNT LOCK */
    pthread_mutex_unlock(&(lock->countMutex));

    /* PERFORM READING */
    result = compare(intsRead, fileLength, buffer[ (threadInfo->counter) % BUFFER_SIZE ]);

    /* IF NEW NUMBER READ ISNT IN THE ARRAY -> ADD IT */
    if( result != FOUND)
    {
        /* COPY BUFFER ELEMENTS INTO ARRAY */
        intsRead[threadInfo->counter] = buffer[ (threadInfo->counter) % BUFFER_SIZE ];
    }
    else /* OTHERWISE OFFSET THE COUNTER */
    {
        threadInfo->counter--;
    }
    /* ACQUIRE COUNT LOCK */
    pthread_mutex_lock(&(lock->countMutex));
      lock->numReaders--;
      if(lock->numReaders == 0)
      {
          /* IF LAST READER, SIGNAL ANY WAITING READERS */
          pthread_cond_signal(&(lock->readersFinished));
          pthread_mutex_unlock(&(lock->rw_mutex));
      }

    /* RELEASE COUNT LOCK */
    pthread_mutex_unlock(&(lock->countMutex));

    threadInfo->counter++;

    /* READER HAS READ ALL THE FILE DATA */
    if( (threadInfo->counter) == fileLength)
    {
      int index = 0;

      /* ACQUIRE FILE COUNT MUTEX LOCK */ 
      pthread_mutex_lock(&(lock->countMutex));
      /* ACQUIRE FILE OUT BUFFER LOCK */
      pthread_mutex_lock(&(lock->outFileMutex));
        index = lock->numThreadsTerminate;
        signoutBuffer[index] = *threadInfo;
        lock->numThreadsTerminate++;
      /* RELEASE FILE OUT BUFFER LOCK */
      pthread_mutex_unlock(&(lock->outFileMutex));
      /* RELEASE COUNT LOCK */
      pthread_mutex_unlock(&(lock->countMutex));

      /* CLEAN UP */
      free(intsRead);
      pthread_exit(NULL);
    }
    else
    {
      sleep(timeout);
    }
  }while(threadInfo->counter != (fileLength));
  return 0;
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
