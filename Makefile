CXX = g++
CXXFLAGS = -std=c++11 -Wall

all: bsp_generator

bsp_generator: main.cpp
	$(CXX) $(CXXFLAGS) -o bsp_generator main.cpp

clean:
	rm -f bsp_generator