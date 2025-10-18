# Compiler and flags
CC = gcc
CFLAGS = -Wall -ansi -pedantic -g

# Libraries
LIBS = -lm

# Executable target
assembler: assembler.o code_converter.o constract.o errors.o firstPhase.o initialize.o labels_Fixer.o macro.o second_Phase.o
	$(CC) $(CFLAGS) assembler.o code_converter.o constract.o errors.o firstPhase.o initialize.o labels_Fixer.o macro.o second_Phase.o -o assembler $(LIBS)

# Object file rules
assembler.o: assembler.c errors.h initialize.h firstPhase.h def.h constract.h
	$(CC) $(CFLAGS) -c assembler.c -o assembler.o

code_converter.o: code_converter.c code_converter.h def.h errors.h labels_Fixer.h macro.h constract.h
	$(CC) $(CFLAGS) -c code_converter.c -o code_converter.o

constract.o: constract.c constract.h def.h errors.h labels_Fixer.h code_converter.h macro.h firstPhase.h initialize.h
	$(CC) $(CFLAGS) -c constract.c -o constract.o

errors.o: errors.c errors.h
	$(CC) $(CFLAGS) -c errors.c -o errors.o

firstPhase.o: firstPhase.c firstPhase.h def.h errors.h constract.h labels_Fixer.h macro.h
	$(CC) $(CFLAGS) -c firstPhase.c -o firstPhase.o

initialize.o: initialize.c initialize.h def.h errors.h macro.h constract.h
	$(CC) $(CFLAGS) -c initialize.c -o initialize.o

labels_Fixer.o: labels_Fixer.c labels_Fixer.h def.h errors.h macro.h constract.h second_Phase.h
	$(CC) $(CFLAGS) -c labels_Fixer.c -o labels_Fixer.o

macro.o: macro.c macro.h def.h errors.h constract.h labels_Fixer.h
	$(CC) $(CFLAGS) -c macro.c -o macro.o

second_Phase.o: second_Phase.c second_Phase.h def.h errors.h constract.h firstPhase.h
	$(CC) $(CFLAGS) -c second_Phase.c -o second_Phase.o

# Clean up object files and the executable
clean:
	rm -f *.o assembler

