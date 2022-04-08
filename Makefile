a4w22: main.o
	g++ -Wall -std=c++11 main.o -o a3w22

main.o: cpp/main.cpp headers/main.h
	g++ -Wall -std=c++11 -c cpp/main.cpp