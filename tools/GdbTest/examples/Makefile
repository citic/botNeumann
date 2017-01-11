PROJECTS=hello_c hello_cpp ftoc_f ftoc_i rand
CC=gcc
CXX=g++

FLAGS=-g -g3 -O0 -Wall -Wextra -I. -pthread
CFLAGS=$(FLAGS) -std=c99
CXXFLAGS=$(FLAGS) -std=c++11

all: $(PROJECTS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

%: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -rf $(PROJECTS) *.exe *.dSYM

clean-dos:
	del /f/q *.exe
