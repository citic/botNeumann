PROJECTS=hello_c hello_cpp ftoc_f ftoc_i rand threads1 input1
CC=gcc
CXX=g++

FLAGS=-g -g3 -O0 -Wall -Wextra -I. -pthread -lpthread
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
