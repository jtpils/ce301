CC = g++

PROG		= main
LIBRARIES   = -lGL -lGLU -lglut

.PHONY = all clean

all: clean main

.cpp:
	$(CC) $@.cpp -o $@ $(LIBRARIES)
	./$@
	
clean:
	$(RM) *.o *~ $(PROG)
