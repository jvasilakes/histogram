CFLAGS = -std=c99 -Wall -pedantic -D_XOPEN_SOURCE=700
LFLAGS = -lm -lncurses
OBJS = roundto.o main.o
PROG = hist
CXX = gcc

all: $(PROG)

%.o: src/%.c src/%.h
	$(CXX) $(CFLAGS) -g -c $<

$(PROG): $(OBJS)
	$(CXX) $(OBJS) -o $(PROG) $(LFLAGS)

clean:
	if [ -f hist ]; then rm hist; fi
	rm *.o
