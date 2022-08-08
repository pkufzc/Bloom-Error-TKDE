CPPFLAGS = -Wall -O3 -std=c++11 -lm -w
PROGRAMS = main

main: main.cpp BF.h CBF.h BOBHash32.h
	g++ -mcmodel=medium -o main main.cpp $(CPPFLAGS)

clean:
	rm -f *.o $(PROGRAMS)
