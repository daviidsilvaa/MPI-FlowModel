
#include "mpi.h"
#include "CellularSpace.hpp"
#include "FlowImpl.hpp"
#include "Model.hpp"
#include "Assert.hpp"
#include "Time.hpp"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <iomanip>
using namespace std;

#define N_F 1000

template<typename T>
class FlowExponencial : public FlowImpl<T>{
public:
	FlowExponencial() : FlowImpl<T>(){ }

	FlowExponencial(const Cell<T> &cell, const double &flow_rate) : FlowImpl<T>(cell, flow_rate){ }

	double execute(){
		return this->flow_rate * this->source.attribute.value;
	}
};

int main(int argc, char *argv[]){
	int comm_rank;
	int comm_size;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);

	ifstream file;
	int i_a, i_b;
	double d_a, d_b;

	file.open(argv[1]);

	file >> d_a;
	file >> d_b;
	Model<double> m1 = Model<double>(d_a, d_b);

	file >> i_a;
	file >> i_b;
	CellularSpace<double> *cs1 = new CellularSpace<double>(i_a, i_b);

	Time t1 = Time();
	t1.getInit();
	cs1->scatter(MPI_COMM_WORLD, LINE);
	t1.getFinish();
	if(comm_rank == MASTER){
		cout << setprecision(10) << t1.getTotalTime() << endl;
	}

	file >> i_a;
	file >> i_b;
	Attribute<double> attrib = Attribute<double>(i_a, i_b);

	for(int i = 0; i < atoi(argv[2]); i++){
		file >> i_a;
		file >> i_b;
		file >> d_a;
		m1.addFlow(new FlowExponencial<double>(Cell<double>(i_a, i_b, attrib), d_a));
	}

	Time t2 = Time();
	t2.getInit();
	m1.execute(MPI_COMM_WORLD, cs1);
	t2.getFinish();
	if(comm_rank == MASTER){
		cout << setprecision(10) << t2.getTotalTime() << endl;
	}

	bool bool_value;

	assert(MPI_COMM_WORLD, cs1, 150, &bool_value);

	// if(comm_rank == MASTER) cout << bool_value << endl;

	// CellularSpace em retangulos
	// CellularSpace<double> *cs2 = new CellularSpace<double>(5, 10);
	// Model<double> m2 = Model<double>(0.5, 0.5);
	//
	// cs2->scatter(MPI_COMM_WORLD, RECTANGULAR);
	//
	// m2.addFlow(new FlowExponencial<double>(cell, 0.1));
	// m2.addFlow(new FlowExponencial<double>(Cell<double>(4, 0, attrib), 0.1));
	// m2.addFlow(new FlowExponencial<double>(Cell<double>(4, 9, attrib), 0.1));
	//
	// m2.execute(MPI_COMM_WORLD, cs2);

	MPI_Finalize();

	return 0;
}
