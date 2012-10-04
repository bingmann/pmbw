
CXXFLAGS = -g -O3 -W -Wall -fopenmp

all:	test

test: funcs.o main.o
	g++ $(CXXFLAGS) -o test $^

main.o: main.cc
	g++ $(CXXFLAGS) -c -o $@ $^

funcs.o: funcs.asm
	nasm -g -f elf64 funcs.asm


clean:
	rm *.o test