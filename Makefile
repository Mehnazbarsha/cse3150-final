CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

.PHONY: all run test clean

all: main tests

main: main.cpp LinkedList.h
	$(CXX) $(CXXFLAGS) -o main main.cpp

tests: tests.cpp LinkedList.h
	$(CXX) $(CXXFLAGS) -o tests tests.cpp

run: main
	./main

test: tests
	./tests

clean:
	rm -f main tests
