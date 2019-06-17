/***************************************************************************
*	FILE: main.c                                                             *
*	AUTHOR: Samantha Taylor                                                  *
*   STUDENT NUMBER: 18863152                                               *
*	UNIT: COMP2006                                                           *
*	PURPOSE: Implementing the first Reader/Writer problem using threads      *
*   LAST MOD: 30/05/18                                                     *
***************************************************************************/
#include "main.h"

int main(int argc, char** argv)
{
  if(argc == 5)
  {
    int numReaders = 0, numWriters = 0, tOne = 0, tTwo = 0;
    numReaders = atoi(argv[1]);
    numWriters = atoi(argv[2]);
    tOne = atoi(argv[3]);
    tTwo = atoi(argv[4]);

    /* VALIDATE INPUT IS POSITIVE */
    if( !(numReaders > -1) || !(numWriters > 0) || !(tOne > 0) || !(tTwo > 0) )
    {
      /* One writer at minimum to write data to buffer */
      printf("ERROR: Number of readers must be 0 or greater\n Number of writers must be 1 or greater\n");
      return -1;
    }
    else
    {
        int status = 0;
        status = startThreads(numReaders, numWriters, tOne, tTwo);
        if(status != 0)
        {
            printf("Error in start threads function\n");
            return -1;
        }
    }
  }
  else
  {
    printf("To use the program: <program name> <num readers> <num writers> <t1> <t2>\n");
  }
  return 0;
}

/* ******************************************************************* *
 * IMPORT: Number of threads to create and their respective timeouts   *
 * EXPORT: Error -1 or 0 for success                                   *
 * PURPOSE: Create correct number of reader and writer threads         *
 *          Assign correct task to thread                              *
 * ******************************************************************* */
int startThreads(int numReaders, int numWriters, int tOne, int tTwo)
{
    int status = 0, fileLength = 0, ii = 0;

    /* SHARED VARIABLE DECLARATIONS */
    Locks* lock = (Locks*)malloc(sizeof(Locks));
    int* buffer = (int*)calloc(BUFFER_SIZE, sizeof(int));
    ThreadInfo* signoutBuffer = (ThreadInfo*)malloc(sizeof(ThreadInfo)*(numReaders + numWriters) );

    /* READ DATA TO GET LENGTH OF FILE */
    fileLength = countFile(FILENAME);

    /* TRANSFER FILE DATA INTO AN ARRAY */
    int* fileData = (int*)calloc(fileLength, sizeof(int));
    readIntoBuffer(FILENAME, fileData);

    /* ARRAY OF THREAD IDS */
    pthread_t writerTIDs[numWriters];
    pthread_t readerTIDs[numReaders];

    /* CREATE LOCKS */
    status = createLocks(lock);
    /* ERROR CHECKING LOCK CREATION */
    if(status != 0)
    {
      printf("ERROR: Occured when creating pthread mutex locks\n");
      return -1;
    }
    /* CREATES NUMBER OF WRITER THREADS NEEDED */
    for( ii = 0; ii < numWriters; ii++ )
    {
        pthread_create(&writerTIDs[ii], NULL, &writer_runner,(void*)(setUpWriter(fileLength, buffer, lock, tTwo, signoutBuffer, fileData, writerTIDs[ii])));
    }

    /* CREATES NUMBER OF READER PROCESSES NEEDED */
    for( ii = 0; ii < numReaders; ii++ )
    {
        pthread_create(&readerTIDs[ii], NULL , &reader_runner, (void*)(setUpReader(fileLength, buffer, lock, tOne, signoutBuffer, readerTIDs[ii])) );
    }

    /* WAITS UNTIL ALL WORKER THREADS HAVE FINISHED THEIR WORK */
    for(ii = 0; ii < numWriters; ii++)
    {
        pthread_join(writerTIDs[ii], NULL);
        /* RELEASES SYSTEM RESOURCES */
        pthread_detach(writerTIDs[ii]);
    }
    for(ii = 0; ii < numReaders; ii++)
    {
        pthread_join(readerTIDs[ii], NULL);
       /* RELEASES SYSTEM RESOURCES */
        pthread_detach(readerTIDs[ii]);
    }

      /* WRITES OUT SIGNOUT BUFFER TO FILE */
      writeOutArray(OUTFILE, signoutBuffer, (numWriters + numReaders));

      /* CLEAN UP OUT BUFFER */
      free(fileData);
      /* REMOVE LOCKS */
      status = deleteLocks(lock);
      /* ERROR CHECKING LOCK DELETION */
      if(status != 0)
      {
        printf("ERROR: Occured when deleting pthread mutex locks\n");
        return -1;
      }
    return 0;
}

/* ************************************************** *
 * Import: infomation only related to writer threads  *
 * Exports: Pointer to a writer struct                *
 * Purpose: Sets up writer struct with correct values *
 * ************************************************** */
WriterStruct* setUpWriter(int fileLength, int* buffer, Locks* lock, int timeout, ThreadInfo* signoutBuffer, int* fileData, int TID)
{
  WriterStruct* writer = (WriterStruct*)malloc(sizeof(WriterStruct));
  writer->fileLength = fileLength;
  writer->buffer = buffer;
  writer->lock = lock;
  writer->timeout = timeout;
  writer->signoutBuffer = signoutBuffer;
  writer->fileData = fileData;
  writer->TID = TID;

  return writer;
}

/* ************************************************** *
 * Import: infomation only related to reader threads  *
 * Exports: Pointer to a reader struct                *
 * Purpose: Sets up reader struct with correct values *
 * ************************************************** */
ReaderStruct* setUpReader(int fileLength, int* buffer, Locks* lock, int timeout, ThreadInfo* signoutBuffer, int TID)
{
  ReaderStruct* reader = (ReaderStruct*)malloc(sizeof(ReaderStruct));
  reader->fileLength = fileLength;
  reader->buffer = buffer;
  reader->lock = lock;
  reader->timeout = timeout;
  reader->signoutBuffer = signoutBuffer;
  reader->TID = TID;

  return reader;
}
/* ************************************************** *
 * Import: pointer to a Lock struct                   *
 * Export: error status -> 0 = correctly set          *
 * Purpose: Set up pthread mutexes                    *
 * ************************************************** */
int createLocks(Locks* lock)
{
  int status = 0;
  status += pthread_mutex_init(&(lock->rw_mutex), NULL);
  status += pthread_mutex_init(&(lock->countMutex), NULL);
  status += pthread_mutex_init(&(lock->outFileMutex), NULL);

  status += pthread_cond_init(&(lock->readersFinished), NULL);
  status += pthread_cond_init(&(lock->writerFinished), NULL);

  lock->numReaders = 0;
  lock->numWriters = 0;
  lock->numThreadsTerminate = 0;

  return status;
}

/* ************************************************** *
 * Import: pointer to a Lock struct                   *
 * Export: error status -> 0 = correctly set          *
 * Purpose: Tear down pthread mutexes                 *
 * ************************************************** */
int deleteLocks(Locks* lock)
{
  int status = 0;
  status += pthread_mutex_destroy(&(lock->rw_mutex));
  status += pthread_mutex_destroy(&(lock->countMutex));
  status += pthread_mutex_destroy(&(lock->outFileMutex));

  status += pthread_cond_destroy(&(lock->readersFinished));
  status += pthread_cond_destroy(&(lock->writerFinished));

  return status;
}
