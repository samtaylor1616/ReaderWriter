CC = gcc
CFLAGS = -Wall -std=c99 -D_XOPEN_SOURCE=500
OBJ = main.o FileIO.o processFunctions.o
EXEC = sdr

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) -pthread -lrt

main.o: main.c main.h FileIO.h processFunctions.h
		$(CC) main.c -c $(CFLAGS) -pthread -lrt

FileIO.o: FileIO.c FileIO.h
		$(CC) FileIO.c -c $(CFLAGS)

processFunctions.o: processFunctions.c processFunctions.h
		$(CC) processFunctions.c -c $(CFLAGS)
clean:
	rm -f $(OBJ) $(EXEC)
