CXX = g++
CXXFLAGS = -g -O2 -W -Wall -fopenmp

all:	pmbw stats2gnuplot
	@echo "To pin threads, run:"
	@echo "export OMP_PROC_BIND=true"

pmbw: main.cc funcs_x86_32.h funcs_x86_64.h funcs_arm.h
	$(CXX) $(CXXFLAGS) -o $@ $<

stats2gnuplot: stats2gnuplot.cc
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f *.o pmbw stats2gnuplot
