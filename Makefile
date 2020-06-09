CC=gcc
CFLAGS = -Wall -Werror -lm
DEPS1 = parasiteFunc.h
DEPS2 = providerFunc.h
OBJ1 = parasiteFunc.o parasite.o
OBJ2 = providerFunc.o provider.o
OBJ3 = familiada.c


parasit%.o: parasit%.c $(DEPS1)
	$(CC) $(CFLAGS) -c -o $@ $<

provide%.o: provide%.c $(DEPS2)
	$(CC) $(CFLAGS) -c -o $@ $<

all: parasite provider familiada


parasite: $(OBJ1)
	$(CC) $(CFLAGS) -o $@ $^
	rm -f *.o

provider: $(OBJ2)
	$(CC) $(CFLAGS) -o $@ $^
	rm -f *.o
	
familiada: $(OBJ3)
	$(CC) $(CFLAGS) -o $@ $(OBJ3)
	
clean:
	rm -f parasite provider familiada
	

