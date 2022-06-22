PROG    := motor
INCDIR  := include
SRCDIR  := src
OBJDIR  := obj
SRCS    := $(wildcard $(SRCDIR)/*.cpp)
OBJS    := $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCS))
CFLAGS  := -std=c++11 -O3 -W -Wall -Wextra -pthread -I$(INCDIR) `pkg-config --cflags --libs slab`

all: $(PROG)

$(OBJDIR)/main.o: main.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) -c $< -o $@ $(CFLAGS) 

$(OBJS):
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) -c $< -o $@ $(CFLAGS) 

$(PROG): $(OBJDIR)/main.o $(OBJS)
	$(CXX) $^ -o $@ $(CFLAGS) 

run: $(PROG)
	@sudo cp ~/.Xauthority /root/
	@sudo ./$< 

clean:
	@$(RM) -rf *~ $(OBJDIR) $(PROG)
	@find -name "*~" | xargs $(RM) -rf

.PHONY: all run clean
