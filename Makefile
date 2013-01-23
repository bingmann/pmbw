CXX = g++
CXXFLAGS = -g -O2 -W -Wall -fopenmp

NASM = nasm
NASMFLAGS = -g -f elf64

all:	pmbw stats2gnuplot
	@echo "To pin threads, run:"
	@echo "export OMP_PROC_BIND=true"

pmbw: funcs.o main.o
	$(CXX) $(CXXFLAGS) -o $@ $^

main.o: main.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $^

funcs.o: funcs.asm
	$(NASM) $(NASMFLAGS) funcs.asm

stats2gnuplot: stats2gnuplot.o
	$(CXX) $(CXXFLAGS) -o $@ $^

stats2gnuplot.o: stats2gnuplot.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $^

clean:
	rm *.o pmbw stats2gnuplot