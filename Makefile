makeT: Globals.cpp  pattern.cpp SO6.cpp Z2.cpp main.cpp
#	g++ main.cpp SO6.cpp Z2.cpp pattern.cpp -std=c++11 -pthread -O3 -o main.out -fopenmp -march=-march='znver2'
	g++ main.cpp SO6.cpp Z2.cpp pattern.cpp Globals.cpp -std=c++11 -pthread -O3 -o main.out -fopenmp -lboost_program_options
#	g++ test_Z2.cpp SO6.cpp Z2.cpp pattern.cpp Globals.cpp -std=c++11 -pthread -O3 -o main.out -fopenmp -lboost_program_options
