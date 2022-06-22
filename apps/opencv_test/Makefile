CFLAGS := `pkg-config --cflags opencv`
LIBS   := `pkg-config --libs opencv`

./test: main.cpp
	g++ $^ $(CFLAGS) $(LIBS) -o test

run: ./test
	./test

clean:
	rm -rf *~ ./test
