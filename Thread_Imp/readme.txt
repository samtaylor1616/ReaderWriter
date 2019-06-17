Student ID: 18863152
Author: Samantha Taylor
Purpose: Thread implementation of first Reader Writer problem.

Instructions for using program:
    Makefile creates a program named sdr.
    Makefile can be viewed via "vim makefile" on terminal.

Usage: "./sdr <numReaders> <numWriters> <readerTimeout> <writerTimeout>"

To use: 
    Number of writer processes must be 1 or greater.
    Number of reader processes must be 0 or greater.
    Both timeouts should be greater than 0.


Files needed for program:
    main.c/h
    FileIO.c/h
    threadFunctions.c/h

Program has been tested with text file shared_data.txt
    File format: 1, 2, 3, 4, 5, 6, 7... 101
    Using: ./sdr 5 2 1 1

Filenames are defined in main.h as
    #define FILENAME "share_data.txt"
    #define OUTFILE "sim_out.txt"

Size of buffer is defined in main.h as
    #define BUFFER_SIZE 20

 
