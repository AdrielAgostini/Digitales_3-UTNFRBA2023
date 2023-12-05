SRCPATH = ./src/
OBJPATH = ./obj/
COMPATH = ./comons/
PUERTO = 10101

CC = gcc
OFLAGS = -lm -o   
CFLAGS = -c -Wall
CFSIMD = -c -Wall -Ofast -pthread -lrt
OUTPUT = Servidor.exe
OBJETO = $(OBJPATH)Servidor.o $(OBJPATH)funcSIMD.o $(OBJPATH)PR_Sensor.o $(OBJPATH)PR_IPC.o $(OBJPATH)config.o $(OBJPATH)string.o $(OBJPATH)dictionary.o

all: $(OBJETO) 
	$(CC) $(OBJETO) $(OFLAGS) $(OUTPUT)	

$(OBJPATH)Servidor.o: $(SRCPATH)Servidor.c
	$(CC) $< $(CFLAGS) -o $@

$(OBJPATH)PR_Sensor.o: $(SRCPATH)PR_Sensor.c
	$(CC) $< $(CFLAGS) -o $@

$(OBJPATH)PR_IPC.o: $(SRCPATH)PR_IPC.c
	$(CC) $< $(CFLAGS) -o $@

$(OBJPATH)funcSIMD.o: $(SRCPATH)funcSIMD.c
	$(CC) $< $(CFSIMD) -o $@

$(OBJPATH)config.o: $(COMPATH)config.c 
	$(CC) $< $(CFLAGS) -o $@

$(OBJPATH)string.o: $(COMPATH)string.c 
	$(CC) $< $(CFLAGS) -o $@

$(OBJPATH)dictionary.o: $(COMPATH)collections/dictionary.c
	$(CC) $< $(CFLAGS) -o $@
	
run:	
	./$(OUTPUT) $(PUERTO)

clean:
	rm ./obj/*.o *.exe 