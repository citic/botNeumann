FLAGS=-ggdb3 -O0 -Wall -Wextra -Wno-uninitialized -I. -pthread -lpthread
CFLAGS=$(FLAGS) -std=c11
CXXFLAGS=$(FLAGS) -std=c++11
TAGS=--c-kinds=v --fields=fsnt --if0=yes --sort=no

alli: *.c *.cpp tags
	g++ $(CXXFLAGS) -o alli.o -c alli.cpp
	gcc $(CFLAGS) -o bn_player_solution.o -c bn_player_solution.c
	g++ $(CXXFLAGS) -o alli alli.o bn_player_solution.o

tags: *.c *.cpp
	ctags $(TAGS) -f tags.txt alli.cpp

clean:
	rm -rf *.o alli tags.txt

tmp:
	cp -p alli.cpp bn_01_input.txt ../bn_player_solution.c Makefile /tmp
	$(MAKE) -C /tmp alli
