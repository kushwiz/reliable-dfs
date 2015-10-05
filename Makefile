CC=gcc
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=dfs.c communicator.c kbcommands.c datapacker.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=dfs

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm dfs *.o
