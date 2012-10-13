CXX = g++
CXXFLAGS = -g -O2 -W -Wall -fopenmp

NASM = nasm
NASMFLAGS = -g -f elf64

all:	pmbw
	@echo "To pin threads, run:"
	@echo "export OMP_PROC_BIND=true"

pmbw: funcs.o main.o
	$(CXX) $(CXXFLAGS) -o pmbw $^

main.o: main.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $^

funcs.o: funcs.asm
	$(NASM) $(NASMFLAGS) funcs.asm

clean:
	rm *.o pmbw