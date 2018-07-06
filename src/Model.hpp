
#ifndef MODEL_HPP
#define MODEL_HPP

#include "mpi.h"
#include "FlowImpl.hpp"
#include "CellularSpace.hpp"
#include <vector>

template<typename T>
class Model{
public:
    std::vector<FlowImpl<T> *> flows;
    double time;
    double time_step;

    Model();
    Model(const double &time, const double &time_step);
    Model(const Model<T> &model);

    Model<T>& operator=(const Model<T> &model);

    ~Model();

    void addFlow(FlowImpl<T> *flow);
    void eraseFlows();

    double execute(const MPI_Comm &mpi_comm, CellularSpace<T> *cs);

private:
    double executeLine(const MPI_Comm &mpi_comm, CellularSpace<T> *cs);
    double executeRectangular(const MPI_Comm &mpi_comm, CellularSpace<T> *cs);
};

#endif
