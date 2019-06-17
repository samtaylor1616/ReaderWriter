/***************************************************************************
*	FILE: FileIO.h
*	AUTHOR: Samantha Taylor
* STUDENT NUMBER: 18863152
*	UNIT: COMP2006
*	PURPOSE: Implementing Reader/Writer problem using threads
*   LAST MOD: 3/05/18
***************************************************************************/
#pragma once

#include <stdio.h>

/* **************************************************************************************** *
 * Counter: is the number of unique characters that have been read or writen by the thread  *
 * TID: is the thread ID number                                                             *
 * threadType: is a char, establishing the type of the thread ('r' for reader and 'w' for   *
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

void readIntoBuffer(char* inFilename, int*);
int countFile(char*);
void writeOutArray(char*, ThreadInfo*, int);
FILE* openFile(char*, char*);
