/***************************************************************************
*	FILE: FileIO.c
*	AUTHOR: Samantha Taylor
* STUDENT NUMBER: 18863152
*	UNIT: COMP2006
*	PURPOSE: Implementing Reader/Writer problem using processes
*   LAST MOD: 23/05/18
***************************************************************************/
#include "FileIO.h"

/* IMPORT: Interger array
 * PURPOSE: Enter the file contents into the array */
void readIntoBuffer(char* inFilename, int* buffer)
{
  FILE* reader = NULL;
  reader = openFile(inFilename, "r");

  if( reader == NULL){printf("Error reading file into buffer, FILE = NULL\n");}
  else
  {
    int ii = 0;
    int ch = fscanf(reader,"%d, ", &(buffer[ii]));
    while( (ch == 1) )
    {
        ii++;
        ch = fscanf(reader,"%d, ", &(buffer[ii]));
    }
  }

  fclose(reader);
}


/* ************************************* *
 * IMPORT: Filename                      *
 * EXPORT: Number of elements in file    *
 * ************************************* */
int countFile(char* filename)
{
  int counter = 0, ch = 0, test = 0;

  /* OPEN FILE AND CHECK IF ERRORS OCCURRED*/
  FILE* file = openFile( filename, "r" );
  if ( file == NULL )
  {
      perror( "ERROR: Opening file\n" );
      return -1;
  }
  ch = fscanf(file,"%d, ", &(test));
  while( (ch == 1) )
  {
    counter++;
    ch = fscanf(file,"%d, ", &(test));
  }

  fclose(file);
  return counter;
}

/* **************************************************** *
 * IMPORT: Out filename, Array of processes information *
 * PURPOSE: Write out process information for reader    *
 *          and writer processes that have completed    *
 *          the task                                    *
 * **************************************************** */
void writeOutArray(char* filename, ProcessInfo* outArray, int arrayLength)
{
    int ii = 0;
    FILE* file = NULL;
    file = openFile(filename, "w");
    if ( file == NULL )
    {
        perror( "ERROR: Opening file\n" );
    }
    for(ii = 0; ii < arrayLength ; ii++)
    {
        if(outArray[ii].processType == 'w')
        {
            fprintf(file, "writer-%d has finished writing %d pieces of data from the data_buffer.\n", outArray[ii].childPID, outArray[ii].counter);
        }
        else if(outArray[ii].processType == 'r')
        {
            fprintf(file, "reader-%d has finished reading %d pieces of data from the data_buffer.\n", outArray[ii].childPID, outArray[ii].counter);
        }
        else
        {
            printf("Error occured in writing \"sim_out\" to file\n");
        }
    }
    fclose(file);
}


/* ******************* File processing functions ****************** */
FILE* openFile(char* inFilename, char* type)
{
  FILE* readFile = NULL;
  readFile = fopen(inFilename, type);
  return readFile;
}
