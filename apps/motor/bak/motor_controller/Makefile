all: main

run: main
	sudo ./main

main: main.cpp
	g++ main.cpp -o main `pkg-config --cflags --libs slab` -lpthread -lm

clean:
	rm -f main
