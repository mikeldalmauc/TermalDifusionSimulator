CC=mpicc
BO=mpiexec
FLAGS=-n
OBJS = faux.o difussion.o 
DIRMP = ../
DIRSERIE = ../ProgramaSerie/aplicacion

all: fauxo difo 
	$(CC) $(OBJS) main.c -o main
	
fauxo:
	$(CC) -c faux.c 
	
difo:
	$(CC) -c difussion.c 	
	
prueba:
	cd $(DIRMP);\
	mpdboot -n 32
	$(BO) $(FLAGS) 32 main card
	$(BO) $(FLAGS) 32 main card
	$(BO) $(FLAGS) 32 main card
	$(BO) $(FLAGS) 32 main card
	$(BO) $(FLAGS) 32 main card
	$(BO) $(FLAGS) 2 main card
	$(BO) $(FLAGS) 2 main card
	$(BO) $(FLAGS) 2 main card
	$(BO) $(FLAGS) 2 main card
	$(BO) $(FLAGS) 2 main card
	$(BO) $(FLAGS) 4 main card
	$(BO) $(FLAGS) 4 main card
	$(BO) $(FLAGS) 4 main card
	$(BO) $(FLAGS) 4 main card
	$(BO) $(FLAGS) 4 main card
	$(BO) $(FLAGS) 8 main card
	$(BO) $(FLAGS) 8 main card
	$(BO) $(FLAGS) 8 main card
	$(BO) $(FLAGS) 8 main card
	$(BO) $(FLAGS) 8 main card
	$(BO) $(FLAGS) 16 main card
	$(BO) $(FLAGS) 16 main card
	$(BO) $(FLAGS) 16 main card
	$(BO) $(FLAGS) 16 main card
	$(BO) $(FLAGS) 16 main card
	$(BO) $(FLAGS) 24 main card
	$(BO) $(FLAGS) 24 main card
	$(BO) $(FLAGS) 24 main card
	$(BO) $(FLAGS) 24 main card
	$(BO) $(FLAGS) 24 main card
	$(BO) $(FLAGS) 32 main card
	$(BO) $(FLAGS) 32 main card
	$(BO) $(FLAGS) 32 main card
	$(BO) $(FLAGS) 32 main card
	$(BO) $(FLAGS) 32 main card
	mpdallexit
	cd $(DIRSERIE);\
	heats card
	
.PHONY: clean

backup:
	cp -R $(DIRMP)Proyect $(DIRMP)ProyectBackup
	
clean:
	rm faux.o difussion.o 
	
cleansolutions:
	rm -f card_ser.chips card_ser.res card0_ser.chips card0_ser.res