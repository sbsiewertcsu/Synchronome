INCLUDE_DIRS = 
LIB_DIRS = 
CC=gcc
#CC=icc

CDEFS=
OMP_CFLAGS= -O0 -fopenmp $(INCLUDE_DIRS) $(CDEFS)
#OMP_CFLAGS= -O0 -qopenmp $(INCLUDE_DIRS) $(CDEFS)
CFLAGS= -O0 $(INCLUDE_DIRS) $(CDEFS)
#CFLAGS= -O0 $(INCLUDE_DIRS) $(CDEFS)
LIBS= -lm

HFILES= 
CFILES= dirparse.c angleparse.c

SRCS= ${HFILES} ${CFILES}
OBJS= ${CFILES:.c=.o}

all:	dirparse angleparse

clean:
	-rm -f *.o *.d
	-rm -f dirparse angleparse

distclean:
	-rm -f *.o *.d
	-rm -f dirparse angleparse

dirparse: dirparse.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $@.o $(LIBS)

angleparse: angleparse.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $@.o $(LIBS)

depend:

.c.o:
	$(CC) $(CFLAGS) -c $<
