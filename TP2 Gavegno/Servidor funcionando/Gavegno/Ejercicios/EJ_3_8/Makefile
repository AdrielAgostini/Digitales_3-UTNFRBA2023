SCRPATH = ./src
OBJPATH = ./obj

CC = gcc
OFLAGS = -lm -o   
CFLAGS = -c -Wall
OUTPUT = ejecutable.exe
OBJETO = main.o handler.o init.o


all: $(OBJETO) 
	$(CC) $(OBJETO) $(OFLAGS) $(OUTPUT)	

main.o: $(SCRPATH)/main.c
	$(CC) $< $(CFLAGS)

handler.o: $(SCRPATH)/handler.c
	$(CC) $< $(CFLAGS)

init.o: $(SCRPATH)/init.c
	$(CC) $< $(CFLAGS)
run:	
	./$(OUTPUT) $(arg)

clean:

	rm *.o *.exe