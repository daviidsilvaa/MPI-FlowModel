
#ifndef CELLULARSPACE_HPP
#define CELLULARSPACE_HPP

#include "Cell.hpp"
#include <vector>

#define LINE 0
#define RECTANGULAR 1

template<class T>
class CellularSpace{
public:
    int x_init;
    int y_init;
    int width;
    int height;
    std::vector<Cell<T> > memoria;
    int type;

    CellularSpace();
    CellularSpace(const int &height, const int &width);
    CellularSpace(const int &x_init, const int &y_init, const int &height, const int &width);

    ~CellularSpace();

    void setXInit(const int &x_init);
    void setYInit(const int &x_init);
    void setHeight(const int &height);
    void setType(const int &type);
    void setWidth(const int &width);

    int getXInit();
    int getYInit();
    int getHeight();
    int getWidth();
    int getSize();
    int getType();

    void scatter(const MPI_Comm &mpi_comm, const int &type);

private:
    void lineScatter(const MPI_Comm &mpi_comm);
    void rectangularScatter(const MPI_Comm &mpi_comm);
};

template<class T>
MPI_Datatype Make_MPI_CellularSpace();

#endif
