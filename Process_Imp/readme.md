# Reader Writer process problem

## Usage:

1. `make`
2. `./sdr <numReaders> <numWriters> <readerTimeout> <writerTimeout>`
    * Makefile creates a program named sdr.
    * Number of writer processes must be 1 or greater.
    * Number of reader processes must be 0 or greater.
    * Both timeouts should be greater than 0.
    
* Program has been tested with text file **shared_data.txt**
    * File format: 1, 2, 3, 4, 5, 6, 7... 101
    * Usage: `./sdr 5 2 1 1`
* Filenames are defined in main.h as
    * `#define FILENAME "share_data.txt"`
    * `#define OUTFILE "sim_out.txt"`
* Size of buffer is defined in main.h as
    * `#define BUFFER_SIZE 20`
 
## Required Files:

* Files needed for program:
    * main.c/h
    * FileIO.c/h
    * processFunctions.c/h
