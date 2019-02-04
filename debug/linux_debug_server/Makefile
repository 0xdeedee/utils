TARGET=debug_server




CC=gcc
DEPS=
CFLAGS= -Wall -O3
INCLUDE= -I../../lists
OBJ:=$(patsubst %.c,%.o,$(wildcard *.c))

LIBS= -lpthread

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

all: clean $(TARGET)

clean:
	rm -rf *~ $(TARGET) $(OBJ) *.txt




