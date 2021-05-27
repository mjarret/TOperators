makeT: SO6.cpp Z2.cpp main.cpp
	g++ -pthread main.cpp SO6.cpp Z2.cpp -O3 -o main.out