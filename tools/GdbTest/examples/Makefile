PROJECTS=hello_c hello_cpp ftoc_f ftoc_i input1 rand rand2 threads1 vars
CC=gcc
CXX=g++

FLAGS=-ggdb3 -Og -Wall -Wextra -Wno-uninitialized -I. -pthread -lpthread
CFLAGS=$(FLAGS) -std=c99
CXXFLAGS=$(FLAGS) -std=c++11

all: $(PROJECTS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $< bn_player_solution.c

%: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< bn_player_solution.cpp

clean:
	rm -rf $(PROJECTS) *.exe *.dSYM

clean-dos:
	del /f/q *.exe
