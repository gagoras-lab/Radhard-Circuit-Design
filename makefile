CC = gcc
CFLAGS = -Wall -g
OBJ = main.o logical_masking.o simulator.o circuit_parser.o #all object files

#top-level goal
all: $(OBJ)
	$(CC) $(OBJ) -o executable -lm

main.o: main.c structs.h functions.h
	$(CC) $(CFLAGS) -c main.c

logical_masking.o: logical_masking.c structs.h functions.h
	$(CC) $(CFLAGS) -c logical_masking.c

simulator.o: simulator.c structs.h functions.h
	$(CC) $(CFLAGS) -c simulator.c

circuit_parser.o: circuit_parser.c structs.h functions.h
	$(CC) $(CFLAGS) -c circuit_parser.c
	
.PHONY: clean
clean:
	rm *.o
