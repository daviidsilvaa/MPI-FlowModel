all:
	mpicxx -std=c++11 src/*.cpp -o exec

run:
	mpirun -np 4 ./exec
