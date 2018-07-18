all:
	mpicxx -std=c++11 src/*.cpp -o exec

run:
	# mpirun -np 3 ./exec instancias/a1_1000.txt 1000
	# mpirun -np 5 ./exec instancias/a2_1000.txt 1000
	# mpirun -np 9 ./exec instancias/a3_1000.txt 1000
	# mpirun -np 3 ./exec instancias/b1_1000.txt 1000
	# mpirun -np 5 ./exec instancias/b2_1000.txt 1000
	# mpirun -np 9 ./exec instancias/b3_1000.txt 1000

	mpirun -np 2 ./exec
	mpirun -np 5 ./exec
	mpirun -np 17 ./exec
