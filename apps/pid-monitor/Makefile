PROG   := main
CFLAGS := -O1 -pthread `pkg-config --cflags --libs slab`
DEFINE := 

all: Makefile $(PROG)

$(PROG): main.cpp
	g++ -std=c++11 $^ -o $@ $(CFLAGS) $(DEFINE)

run: $(PROG)
	sudo ./$^ 

clean:
	@find . -name "*~" | xargs rm -rf 
	@rm -f ./$(PROG) *~

.PHONY: all run clean
