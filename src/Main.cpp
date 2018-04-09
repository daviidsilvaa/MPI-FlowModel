
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
	Cell<double> cell = Cell<double>(4, 9, attrib);
	CellularSpace<double> *cs = new CellularSpace<double>(5, 10);

	cs->lineScatter(MPI_COMM_WORLD);

	// if(comm_rank == 0)
	// 	for(int i = 0; i < cs->getHeight()*cs->getWidth(); i++)
	// 		cout << cs->memoria[i].getX() << "\t" << cs->memoria[i].getY() << "\t" << cs->memoria[i].count_neighbors << endl;

	FlowImpl<double> *f1 = new FlowExponencial<double>(cell, 0.1);

	Model<double> m1 = Model<double>(0.5, 0.5);

	m1.addFlow(f1);
	m1.execute(MPI_COMM_WORLD, cs);

	MPI_Finalize();

	return 0;
}
