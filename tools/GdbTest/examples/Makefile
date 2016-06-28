CC=gcc
CXX=g++

FLAGS=-Wall -g -O0 -I.
CFLAGS=$(FLAGS) -std=c99
CXXFLAGS=$(FLAGS) -std=c++11

all: hello_c hello_cpp ftoc_f ftoc_i

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

%: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -rf hello_c hello_cpp *.exe *.dSYM

clean-dos:
	del /f/q *.exe
