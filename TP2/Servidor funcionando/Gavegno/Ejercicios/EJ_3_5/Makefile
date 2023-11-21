SCRPATH = ./scr
OBJPATH = ./obj

CC = gcc
OFLAGS = -lm -o   
CFLAGS = -c -Wall
OUTPUT = ejecutable.exe
OBJETO = main.o handler.o


all: $(OBJETO) 
	$(CC) $(OBJETO) $(OFLAGS) $(OUTPUT)	

main.o: $(SCRPATH)/main.c
	$(CC) $< $(CFLAGS)

handler.o: $(SCRPATH)/handler.c
	$(CC) $< $(CFLAGS)

run:	
	./$(OUTPUT)

clean:

	rm *.o *.exe