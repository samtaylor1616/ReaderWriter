/***************************************************************************
*	FILE: FileIO.h
*	AUTHOR: Samantha Taylor
* STUDENT NUMBER: 18863152
*	UNIT: COMP2006
*	PURPOSE: Implementing Reader/Writer problem using processes
*   LAST MOD: 23/05/18
***************************************************************************/
#pragma once

#include <stdio.h>

#ifndef PROCESS_INFO_GUARD
#define PROCESS_INFO_GUARD
  typedef struct{
    int counter;
    int childPID;
    char processType;
  }ProcessInfo;
#endif

void readIntoBuffer(char* inFilename, int*);
int countFile(char*);
void writeOutArray(char*, ProcessInfo*, int);
FILE* openFile(char*, char*);
