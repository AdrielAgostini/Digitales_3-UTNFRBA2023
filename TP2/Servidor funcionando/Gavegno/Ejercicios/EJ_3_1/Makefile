SCRPATH = ./scr
OBJPATH = ./obj

CC = gcc
OFLAGS = -lm -o   
CFLAGS = -c -Wall
OUTPUT = ejecutable.exe
SOURCE = main.c
OBJETO = main.o


all: $(OBJETO) 
	$(CC) $(OBJETO) $(OFLAGS) $(OUTPUT)	

main.o: $(SCRPATH)/main.c
	$(CC) $< $(CFLAGS)

run:	
	./$(OUTPUT)

clean:

	rm *.o *.exe