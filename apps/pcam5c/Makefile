PROG   := main
CFLAGS := -O1 -pthread `pkg-config --cflags --libs slab slab_pcam opencv`
DEFINE := 

all: Makefile $(PROG)

$(PROG): main.cpp
	g++ -std=c++11 $^ -o $@ $(CFLAGS) $(DEFINE)

run: $(PROG)
	sudo cp ~/.Xauthority /root/
	sudo ./$^ 2>/dev/null

clean:
	@find . -name "*~" | xargs rm -rf 
	@rm -f ./$(PROG) *~

.PHONY: all run clean
