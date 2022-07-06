CC=gcc

CFLAGS  = `pkg-config --libs slab_pcam slab` -Wall

all: testmem

testmem: testmem.c
	$(CC) $(CFLAGS) -o $@ $<

clean: testmem
	rm -rf testmem
