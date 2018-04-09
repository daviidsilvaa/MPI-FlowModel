
#ifndef CELL_HPP
#define CELL_HPP

#include "Defines.hpp"
#include "Attribute.hpp"

template<typename T>
class Cell{
public:
    int x;
    int y;
    Attribute<T> attribute;
    // Cell<T> neighbors[NEIGHS];
    int neighbors[NEIGHBORS + NEIGHBORS];
    int count_neighbors;

    Cell();

    Cell(const int &x, const int &y, const Attribute<T> &attrib);

    Cell(const Cell<T> &cell);

    ~Cell();

    Cell<T>& operator=(const Cell<T> &cell);

    void setX(const int &x);
    void setY(const int &y);
    void setAttribute(const Attribute<T> &attrib);

    int getX();
    int getY();
    Attribute<T> getAttribute();

    Cell<T> setNeighbor(const int &height, const int &width);
};

// template<typename T>
// MPI_Datatype Make_MPI_Cell(){
//     MPI_Datatype MPI_Attribute = Make_MPI_Atribute<T>();
//
//     MPI_Datatype MPI_Cell;
//
//     const int nitens = 5;
//     MPI_Datatype blocktypes[nitens] = {MPI_INT, MPI_INT, MPI_Attribute, MPI_INT, MPI_INT};
//     int blocklengths[nitens] = {1, 1, 1, (NEIGHBORS + NEIGHBORS), 1};
//     MPI_Aint offsets[nitens] = {offsetof(Cell<T>, x), offsetof(Cell<T>, y), offsetof(Cell<T>, attribute),
//         offsetof(Cell<T>, neighbors), offsetof(Cell<T>, count_neighbors)};
//
//     MPI_Type_create_struct(nitens, blocklengths, offsets, blocktypes, &MPI_Cell);
//     MPI_Type_commit(&MPI_Cell);
//
//     return MPI_Cell;
// }

#endif

// void AddNeighbor(const int &x_neighbor, const int &y_neighbor){
//     this->neighbors[count_neighbors] = x_neighbor;
//     this->neighbors[NEIGHBORS + this->count_neighbors] = y_neighbor;
//     this->count_neighbors = this->count_neighbors + 1;
// }
