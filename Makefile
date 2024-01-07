CFLAGS=-std=c++17
SRCS=sudoku.cpp sa.cpp backtrack.cpp opHint.cpp algoX.cpp
OBJS=$(SRCS:.cpp=.o)
CC=g++

sudoku: $(OBJS)
	$(CC) -O4 -o sudoku.exe $(OBJS) $(CFLAGS)

$(OBJS): 
	$(CC) -O4 -flto -o $@ $(CFLAGS) -c $*.cpp 

clean:
	rm -f sudoku.exe *.o
	make

.PHONY: clean
