SRCPATH = ./src
OBJPATH = ./obj

CC = gcc
OFLAGS = -lm -o   
CFLAGS = -c -Wall
OUTPUT = ejecutable.exe
OBJETO = mainUDP.o 


all: $(OBJETO) 
	$(CC) $(OBJETO) $(OFLAGS) $(OUTPUT)	

main.o: main.c
	$(CC) $< $(CFLAGS)

run:	
	./$(OUTPUT) $(arg)

clean:

	rm *.o *.exe