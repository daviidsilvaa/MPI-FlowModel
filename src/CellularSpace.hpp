
#ifndef CELLULARSPACE_HPP
#define CELLULARSPACE_HPP

#include "Cell.hpp"
#include "Defines.hpp"

#define LINE 0
#define RECTANGULAR 1

template<class T>
class CellularSpace{
public:
    int x_init;
    int y_init;
    int width;
    int height;
    Cell<T> memoria[PROC_DIMX*PROC_DIMY];
    int type;

    CellularSpace();
    CellularSpace(const int &height, const int &width);
    CellularSpace(const int &x_init, const int &y_init, const int &height, const int &width);

    ~CellularSpace();

    void setXInit(const int &x_init);
    int getXInit() const;
    void setYInit(const int &x_init);
    int getYInit() const;
    void setHeight(const int &height);
    int getHeight() const;
    void setWidth(const int &width);
    int getWidth() const;

    void setType(const int &type);
    int getType() const;

    void lineScatter(const MPI_Comm &mpi_comm);
};

template<class T>
MPI_Datatype Make_MPI_CellularSpace();

#endif
