CC = gcc
CFLAGS = -Wall -std=c99 -D_XOPEN_SOURCE=500
OBJ = main.o FileIO.o threadFunctions.o
EXEC = sdr

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) -pthread

main.o: main.c main.h FileIO.h threadFunctions.h
		$(CC) main.c -c $(CFLAGS) -pthread

FileIO.o: FileIO.c FileIO.h
		$(CC) FileIO.c -c $(CFLAGS)

threadFunctions.o: threadFunctions.c threadFunctions.h
		$(CC) threadFunctions.c -c $(CFLAGS)
clean:
	rm -f $(OBJ) $(EXEC)
