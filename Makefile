a4w22: main.o
	g++ -Wall -pthread -std=c++11 main.o -o a4w22

main.o: cpp/main.cpp headers/main.h
	g++ -Wall -pthread -std=c++11 -c cpp/main.cpp

clean:
	rm -f *a4w22 *.o *.tar

tar:
	tar -czf sheng-a4.tar *