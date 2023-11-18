# Fichier Makefile

CC = gcc
CFLAGS = -g -Wall -std=c11  
CFLAG2 = -c -Wall -std=c11 
OBJETS = SV.o SV_mem.o SV_sem.o

SV : $(OBJETS)
	$(CC) $(CFLAGS) $(OBJETS) -o SV

SV.o : SV.c SV_def.h
	$(CC) $(CFLAG2) SV.c -o SV.o

SV_mem.o : SV_mem.c SV_mem.h SV_def.h 
	$(CC) $(CFLAG2) SV_mem.c -o SV_mem.o

SV_sem.o : SV_sem.c SV_sem.h  SV_def.h
	$(CC) $(CFLAG2) SV_sem.c -o SV_sem.o

clean:
	@rm -f *~ *.o SV 




