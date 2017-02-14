FLAGS=-ggdb3 -O0 -Wall -Wextra -Wno-uninitialized -I. -pthread -lpthread
CFLAGS=$(FLAGS) -std=c11
CXXFLAGS=$(FLAGS) -std=c++11
TAGS=--c++-kinds=vf --fields=fKsnt --if0=yes --sort=no

alli: *.cpp tags
	g++ $(CXXFLAGS) -o alli.o -c alli.cpp
	gcc $(CFLAGS) -o bn_player_solution.o -c ../bn_player_solution.c
	g++ $(CXXFLAGS) -o alli alli.o bn_player_solution.o

tags: bn_tags.txt

bn_tags.txt: *.cpp
	ctags $(TAGS) -f bn_tags.txt alli.cpp

clean:
	rm -rf *.o alli tags.txt

tmp:
	cp -p alli.cpp bn_01_input.txt ../bn_player_solution.c Makefile /tmp
	$(MAKE) -C /tmp alli
