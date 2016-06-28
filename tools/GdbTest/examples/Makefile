all: hello_c hello_cpp
# params_c params_cpp

hello_c: hello_c.c
	cc -Wall -std=c99 -g -O0 -o hello_c hello_c.c

hello_cpp: hello_cpp.cpp
	c++ -Wall -std=c++11 -g -O0 -o hello_cpp hello_cpp.cpp

clean:
	rm -rf hello_c hello_cpp *.dSYM
