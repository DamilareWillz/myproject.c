CC=g++
CFLAGS=-Wall --std=c++11

all: cpe351

cpe351: cpe351.o taskqueue.o
	$(CC) $? -o $@

.cpp.o: 
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f cpe351 *.o