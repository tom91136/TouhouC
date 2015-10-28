#taken from http://stackoverflow.com/questions/1484817/how-do-i-make-a-simple-makefile-gcc-unix
#the source did not specify a license, stackoverflow says the code is CC, so doing this is fine



TARGET = main
LIBS = -lm
CC = gcc
CFLAGS = -g -std=c99 -O3
SRC=src/

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.c, %.o, $(wildcard $(SRC)*.c))
HEADERS = $(wildcard $(SRC)*.h)

%.o: %.c $(SRC)$(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(SRC) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS)  $(LIBS) -o $@

clean:
	-rm -f $(SRC)*.o
	-rm -f $(TARGET)