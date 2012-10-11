CXX = g++
CXXFLAGS = -g -O2 -W -Wall -fopenmp

NASM = nasm
NASMFLAGS = -g -f elf64

all:	pmbw

pmbw: funcs.o main.o
	$(CXX) $(CXXFLAGS) -o pmbw $^

main.o: main.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $^

funcs.o: funcs.asm
	$(NASM) $(NASMFLAGS) funcs.asm

clean:
	rm *.o pmbw