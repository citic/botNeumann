PROJECTS=all_inclusive/alli hello_c hello_cpp ftoc_f ftoc_i heap1 input1 rand rand2 threads1 threads2 vars tags
CC=gcc
CXX=g++

FLAGS=-ggdb3 -O0 -Wall -Wextra -Wno-uninitialized -I. -pthread -lpthread
CFLAGS=$(FLAGS) -std=c11
CXXFLAGS=$(FLAGS) -std=c++11
TAGS=--c-kinds=v --fields=fsnt --if0=yes --sort=no

all: $(PROJECTS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $< bn_player_solution.c

%: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< bn_player_solution.cpp

# ToDo: include header files in tags
tags: *.c *.cpp
	ctags $(TAGS) -f tags.txt *.c *.cpp
	ctags $(TAGS) -f all_inclusive/tags.txt all_inclusive/alli.cpp

clean:
	rm -rf $(PROJECTS) *.exe *.dSYM tags*

clean-dos:
	del /f/q *.exe
