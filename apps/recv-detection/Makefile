APP := recv_detection

all: $(APP)

$(APP): main.cpp
	g++ $< kbhit.o -o $@ `pkg-config --cflags --libs slab`

run: $(APP)
	sudo ./$(APP)

clean:
	rm -rf *~ ./$(APP)
