qTARGET = server
LIBS = -lm
CC = gcc
CFLAGS = -g -Wall -D_BSD_SOURCE -std=c99

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.c, %.o, $(shell find  -type f -name '*.c'))
HEADERS = $(shell find  -type f -name '*.h')

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	$(shell find  -type f -name '*.o' -delete)
	-rm -f $(TARGET)