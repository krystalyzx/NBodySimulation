CC = mpicc
RM = rm -f

OBJECTS = nbody.o
PROG = nbody

nbody : nbody.c
	$(CC) -o nbody  $(CFLAGS) $(INCLUDES) nbody.c -lm

clean:
	$(RM) $(OBJECTS) *~ *.o core $(PROG)