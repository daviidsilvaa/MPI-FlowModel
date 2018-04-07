
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

	Attribute<double> attrib = Attribute<double>(1, 0.2);
	Cell<double> cell = Cell<double>(19, 5, attrib);
	CellularSpace<double> cs = CellularSpace<double>(20, 40);

	cs.LineScatter(MPI_COMM_WORLD);

	FlowImpl<double> *f1 = new FlowExponencial<double>(cell, 0.1);

	Model<double> m1 = Model<double>(0.5, 0.5);

	m1.addFlow(f1);
	m1.execute(MPI_COMM_WORLD, cs);

	MPI_Finalize();

	return 0;
}
