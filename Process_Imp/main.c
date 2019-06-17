/***************************************************************************
*	FILE: main.c                                                           *  
*	AUTHOR: Samantha Taylor                                                *
*   STUDENT NUMBER: 18863152                                               *
*	UNIT: COMP2006                                                         *
*	PURPOSE: Implementing the first Reader/Writer problem using processes  *
*   LAST MOD: 30/05/18                                                     *
***************************************************************************/
#include "main.h"

int main(int argc, char** argv)
{
  if(argc == 5)
  {
    int numReaders = 0, numWriters = 0, tOne = 0, tTwo = 0, status = 0;
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
        status = startProcess(numReaders, numWriters, tOne, tTwo);
        if(status != 0)
        {
            printf("Error in start process function\n");
            return -1;
        }
    }
  }
  else
  {
    printf("To use the program: <program name> <num readers> <num writers> <t1> <t2>");
  }
  return 0;
}

/* ******************************************************************* *
 * IMPORT: Number of processes to create and their respective timeouts *
 * EXPORT: Error -1 or 0 for success                                   *
 * PURPOSE: Create correct number of reader and writer processes       *
 *          Assign correct task to process                             *
 * ******************************************************************* */
int startProcess(int numReaders, int numWriters, int tOne, int tTwo) 
{
    /* SHARED VARIABLE DECLARATIONS */
    Locks* lock;
    int* buffer;
    ProcessInfo* signoutBuffer;

    /* SHARED MEMORY FILE DESCRIPTOR */
    int locksFD, bufferFD, outFileFD;
    int status = 0, wstatus = 0, fileLength = 0, pid = -1, parentPID = 0, ii = 0;
    parentPID = getpid();

    /* READ DATA TO GET LENGTH OF FILE */
    fileLength = countFile(FILENAME);

    /* TRANSFER FILE DATA INTO AN ARRAY */
    int* fileData = (int*)calloc(fileLength, sizeof(int));
    readIntoBuffer(FILENAME, fileData);

    /* OPEN SHARED MEMORY OBJECT */
    bufferFD = shm_open("data_buffer", O_CREAT | O_RDWR, 0666 );
    locksFD = shm_open( "semaphore_lock", O_CREAT | O_RDWR, 0666 );
    outFileFD = shm_open("sim_out", O_CREAT | O_RDWR, 0666 );
    
    /* ERROR CHECKING MEMORY OPENED */
    if( (bufferFD == -1) || (locksFD == -1) || (outFileFD == -1))
    {
        printf("Error is: %s\n", strerror(errno));
        printf("ERROR: Occured when creating memory blocks\n");
        return -1;
    }

    /* TRUNCATE SEGMENTS TO CORRECT SIZE */
    status += ftruncate( bufferFD, sizeof(int) * BUFFER_SIZE);
    status += ftruncate( locksFD, sizeof(Locks) );
    status += ftruncate( outFileFD, sizeof(ProcessInfo) * (numReaders + numWriters) );

    /* ERROR CHECKING TRUNCATED SEGMENTS */
    if(status != 0)
    {
      printf("Error is: %s\n", strerror(errno));
      printf("ERROR: Occured when truncating size memory blocks\n");
      return -1;
    }

    /* MEMORY MAPPED THE SHARED MEMORY OBJECT */
    buffer = (int*)mmap(0, (sizeof(int) * BUFFER_SIZE), PROT_READ | PROT_WRITE, MAP_SHARED, bufferFD, 0);
    lock = (Locks*)mmap(0, sizeof(Locks), PROT_READ | PROT_WRITE, MAP_SHARED, locksFD, 0);
    signoutBuffer = (ProcessInfo*)mmap(0, sizeof(ProcessInfo) * (numWriters + numReaders), PROT_READ | PROT_WRITE, MAP_SHARED, outFileFD, 0);

    status = createLocks(lock);
    /* ERROR CHECKING LOCKS CREATION */
    if(status != 0)
    {
      printf("Error is: %s\n", strerror(errno));
      printf("ERROR: Occured when setting semaphore locks\n");
      return -1;
    }

    /* SIGNAL TO AVOID CREATION OF ZOMBIE PROCESSES
     * SIGCHLD is the termination signal of the child */
    signal(SIGCHLD, SIG_IGN);

    /* CREATES NUMBER OF WRITER PROCESSES NEEDED */
    for( ii = 0; ii < numWriters; ii++ )
    {
      /* ONLY PARENT PROCESS CREATES WRITER PROCESSES */
      if( parentPID == getpid() )
      {
          pid = fork();
      }
      /* CHILDREN RUN WRITER FUNCTION */
      if( pid == 0 )
      {
          writer(fileLength, buffer, lock, tTwo, signoutBuffer, fileData);
      }
    }

    /* CREATES NUMBER OF READER PROCESSES NEEDED */
    for( ii = 0; ii < numReaders; ii++ )
    {
      /* ONLY PARENT CREATES READER PROCESSES */
      if( parentPID == getpid() )
      {
          pid = fork();
      }
      /* CHILDREN RUN READER FUNCTION */
      if( pid == 0 )
      {
          reader(fileLength, buffer, lock, tOne, signoutBuffer);
      }
    }

    /* ONLY PARENT PROCESS EXECUTES */
    if( parentPID == getpid() )
    {
      /* WAITS FOR ALL CHILDREN TO TERMINATE */
      for(ii = 0; ii < (numReaders + numWriters); ii++)
      {
        wait(&wstatus); 
      }

      /* WRITES OUT SIGNOUT BUFFER TO FILE */
      writeOutArray(OUTFILE, signoutBuffer, (numWriters + numReaders));

      /* CLEAN UP OUT BUFFER */
      free(fileData);

      /* REMOVE SEMAPHORES */
      status = deleteLocks(lock);
      /* ERROR CHECKING LOCK DELETION */
      if(status != 0)
      {
        printf("ERROR: Occured when deleting semaphore locks\n");
        return -1;
      }

      /* REMOVE SHARED MEMORY */
      status += shm_unlink("data_buffer");
      status += shm_unlink("semaphore_lock");
      status += shm_unlink("sim_out");
      /* ERROR CHECKING REMOVAL OF SHARED MEMORY */
      if(status != 0)
      {
        printf("ERROR: Occured when deallocating memory blocks\n");
        return -1;
      } 
    }

    return 0;
}

/* Import: pointer to a Lock struct
 * Export: error status -> 0 = correctly set
 * Purpose: Set up mutex and semaphore locks */
int createLocks(Locks* lock)
{
  int status = 0;
  status += sem_init(&(lock->rw_mutex), 1, 1 );
  status += sem_init(&(lock->countMutex), 1, 1);
  status += sem_init(&(lock->outFileMutex), 1, 1);
  lock->numReaders = 0;
  lock->numProcessesTerminate = 0;

  return status;
}
/* Import: pointer to a Lock struct
 * Export: error status -> 0 = correctly set
 * Purpose: Tear down mutex and semaphore locks */
int deleteLocks(Locks* lock)
{
  int status = 0;
  status += sem_destroy(&(lock->rw_mutex));
  status += sem_destroy(&(lock->countMutex));
  status += sem_destroy(&(lock->outFileMutex));
  return status;
}

