SCRPATH = ./scr
OBJPATH = ./obj

CC = gcc
OFLAGS = -lm -o   
CFLAGS = -c -Wall
OUTPUT = ejecutable.exe
SOURCE = funciones.c main.c
OBJETO = main.o funciones.o 


all: $(OBJETO) 
	$(CC) $(OBJETO) $(OFLAGS) $(OUTPUT)	

funciones.o: $(SCRPATH)/funciones.c
	$(CC) $< $(CFLAGS)

main.o: $(SCRPATH)/main.c
	$(CC) $< $(CFLAGS)

run:	
	./$(OUTPUT) $(arg)

clean:

	rm *.o *.exe