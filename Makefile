CXX      := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra

TARGETS := basic efficient

all: $(TARGETS)

basic: basic.cpp
	$(CXX) $(CXXFLAGS) basic.cpp -o basic

efficient: efficient.cpp
	$(CXX) $(CXXFLAGS) efficient.cpp -o efficient

run-basic: basic
	./basic Datapoints/in1.txt outputs/out_basic.txt

run-efficient: efficient
	./efficient Datapoints/in1.txt outputs/out_efficient.txt

clean:
	rm -f $(TARGETS) *.o 
