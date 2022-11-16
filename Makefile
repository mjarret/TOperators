makeT: SO6.cpp Z2.cpp main.cpp
	g++ main.cpp SO6.cpp Z2.cpp pattern.cpp -std=c++17 -pthread -O3 -o main.out -fopenmp
