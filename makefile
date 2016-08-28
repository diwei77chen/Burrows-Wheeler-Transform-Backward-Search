all: bwtsearch

bwtsearch: bwtsearch.o
	g++ bwtsearch.o -o bwtsearch

bwtsearch.o: bwtsearch.cpp bwtsearch.h
	g++ -std=c++11 -Wall -Werror -O2 -c -g bwtsearch.cpp 
