
#include "mpi.h"
#include "CellularSpace.hpp"
#include "FlowImpl.hpp"
#include "Model.hpp"
#include <iostream>
using namespace std;

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

	Attribute<double> attrib = Attribute<double>(1, 1);
	// Cell<double> cell = Cell<double>(4, 0, attrib);
	Model<double> m1 = Model<double>(0.5, 0.5);
	// CellularSpace em linhas
	CellularSpace<double> *cs1 = new CellularSpace<double>(5, 10);
	cs1->scatter(MPI_COMM_WORLD, LINE);

	double sum1 = 0;
	for(int i = 0; i < cs1->getSize(); i++){
		sum1 += cs1->memoria[i].attribute.value;
	}

	// m1.addFlow(new FlowExponencial<double>(cell, 0.1));

	m1.addFlow(new FlowExponencial<double>(Cell<double>(4, 0, attrib), 0.1));
	// if(comm_rank == 0) cout << "f1 adicionado" << endl;
	m1.addFlow(new FlowExponencial<double>(Cell<double>(4, 9, attrib), 0.1));
	// if(comm_rank == 0) cout << "f2 adicionado" << endl;
	m1.addFlow(new FlowExponencial<double>(Cell<double>(9, 0, attrib), 0.1));
	// if(comm_rank == 0) cout << "f3 adicionado" << endl;
	m1.addFlow(new FlowExponencial<double>(Cell<double>(9, 9, attrib), 0.1));
	// if(comm_rank == 0) cout << "f4 adicionado" << endl;
	m1.addFlow(new FlowExponencial<double>(Cell<double>(0, 0, attrib), 0.1));
	// if(comm_rank == 0) cout << "f5 adicionado" << endl;
	m1.addFlow(new FlowExponencial<double>(Cell<double>(0, 9, attrib), 0.1));
	// if(comm_rank == 0) cout << "f6 adicionado" << endl;
	m1.addFlow(new FlowExponencial<double>(Cell<double>(3, 5, attrib), 0.1));
	// if(comm_rank == 0) cout << "f7 adicionado" << endl;
	m1.addFlow(new FlowExponencial<double>(Cell<double>(7, 5, attrib), 0.1));
	// if(comm_rank == 0) cout << "f8 adicionado" << endl;
	m1.addFlow(new FlowExponencial<double>(Cell<double>(12, 5, attrib), 0.1));
	// if(comm_rank == 0) cout << "f9 adicionado" << endl;
	m1.addFlow(new FlowExponencial<double>(Cell<double>(3, 0, attrib), 0.1));
	// if(comm_rank == 0) cout << "f10 adicionado" << endl;
	m1.addFlow(new FlowExponencial<double>(Cell<double>(3, 9, attrib), 0.1));
	// if(comm_rank == 0) cout << "f11 adicionado" << endl;
	m1.addFlow(new FlowExponencial<double>(Cell<double>(7, 0, attrib), 0.1));
	// if(comm_rank == 0) cout << "f12 adicionado" << endl;
	m1.addFlow(new FlowExponencial<double>(Cell<double>(7, 9, attrib), 0.1));
	// if(comm_rank == 0) cout << "f13 adicionado" << endl;
	m1.addFlow(new FlowExponencial<double>(Cell<double>(12, 0, attrib), 0.1));
	// if(comm_rank == 0) cout << "f14 adicionado" << endl;
	m1.addFlow(new FlowExponencial<double>(Cell<double>(12, 9, attrib), 0.1));
	// if(comm_rank == 0) cout << "f15 adicionado" << endl;
	

	m1.execute(MPI_COMM_WORLD, cs1);

	double sum2 = 0;
	for(int i = 0; i < cs1->getSize(); i++){
		sum2 += cs1->memoria[i].attribute.value;
	}

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
