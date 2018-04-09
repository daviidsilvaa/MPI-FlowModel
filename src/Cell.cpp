
#include "Cell.hpp"
#include "MPIImpl.hpp"

template<class T>
Cell<T>::Cell(){ }

template<class T>
Cell<T>::Cell(const int &x, const int &y, const Attribute<T> &attrib){
    this->setX(x);
    this->setY(y);
    this->setAttribute(attrib);
    this->count_neighbors = 0;
}

template<class T>
Cell<T>::Cell(const Cell<T> &cell){
    this->setX(cell.x);
    this->setY(cell.y);
    this->setAttribute(cell.attribute);
    this->count_neighbors = cell.count_neighbors;
    for(int i = 0; i < NEIGHBORS; i++)
    this->neighbors[i] = cell.neighbors[i];
}

template<class T>
Cell<T>::~Cell(){ }

template<class T>
Cell<T>& Cell<T>::operator=(const Cell<T> &cell){
    if(this != &cell){
        this->setX(cell.x);
        this->setY(cell.y);
        this->setAttribute(cell.attribute);
        this->count_neighbors = cell.count_neighbors;
        for(int i = 0; i < NEIGHBORS; i++)
        this->neighbors[i] = cell.neighbors[i];
    }
    return *this;
}

template<class T>
void Cell<T>::setX(const int &x){
    this->x = x;
}
template<class T>
void Cell<T>::setY(const int &y){
    this->y = y;
}
template<class T>
void Cell<T>::setAttribute(const Attribute<T> &attrib){
    this->attribute = attrib;
}

template<class T>
int Cell<T>::getX(){
    return this->x;
}
template<class T>
int Cell<T>::getY(){
    return this->y;
}
template<class T>
Attribute<T> Cell<T>::getAttribute(){
    return this->attribute;
}

template<class T>
Cell<T> Cell<T>::setNeighbor(const int &height, const int &width){
    Cell<T> cell = *this;

    if(cell.x == 0){
        if(cell.y == 0){
            cell.count_neighbors = 3;
            cell.neighbors[0] = cell.x;      cell.neighbors[NEIGHBORS + 0] = cell.y + 1;
            cell.neighbors[1] = cell.x + 1;  cell.neighbors[NEIGHBORS + 1] = cell.y + 1;
            cell.neighbors[2] = cell.x + 1;  cell.neighbors[NEIGHBORS + 2] = cell.y;
            return cell;
        }
        if(cell.y == width-1){
            cell.count_neighbors = 3;
            cell.neighbors[0] = cell.x;      cell.neighbors[NEIGHBORS + 0] = cell.y - 1;
            cell.neighbors[1] = cell.x + 1;  cell.neighbors[NEIGHBORS + 1] = cell.y - 1;
            cell.neighbors[2] = cell.x + 1;  cell.neighbors[NEIGHBORS + 2] = cell.y;
            return cell;
        }
        if(cell.y != width-1 && cell.y != 0){
            cell.count_neighbors = 5;
            cell.neighbors[0] = cell.x;      cell.neighbors[NEIGHBORS + 0] = cell.y + 1;
            cell.neighbors[1] = cell.x + 1;  cell.neighbors[NEIGHBORS + 1] = cell.y + 1;
            cell.neighbors[2] = cell.x + 1;  cell.neighbors[NEIGHBORS + 2] = cell.y;
            cell.neighbors[3] = cell.x + 1;  cell.neighbors[NEIGHBORS + 3] = cell.y - 1;
            cell.neighbors[4] = cell.x;      cell.neighbors[NEIGHBORS + 4] = cell.y - 1;
            return cell;
        }
    }
    if(cell.x == height-1){
        if(cell.y == 0){
            cell.count_neighbors = 3;
            cell.neighbors[0] = cell.x - 1;  cell.neighbors[NEIGHBORS + 0] = cell.y;
            cell.neighbors[1] = cell.x - 1;  cell.neighbors[NEIGHBORS + 1] = cell.y - 1;
            cell.neighbors[2] = cell.x;      cell.neighbors[NEIGHBORS + 2] = cell.y + 1;
            return cell;
        }
        if(cell.y == width-1){
            cell.count_neighbors = 3;
            cell.neighbors[0] = cell.x - 1;  cell.neighbors[NEIGHBORS + 0] = cell.y;
            cell.neighbors[1] = cell.x - 1;  cell.neighbors[NEIGHBORS + 1] = cell.y - 1;
            cell.neighbors[2] = cell.x;      cell.neighbors[NEIGHBORS + 2] = cell.y - 1;
            return cell;
        }
        if(cell.y != width-1 && cell.y != 0){
            cell.count_neighbors = 5;
            cell.neighbors[0] = cell.x;      cell.neighbors[NEIGHBORS + 0] = cell.y + 1;
            cell.neighbors[1] = cell.x - 1;  cell.neighbors[NEIGHBORS + 1] = cell.y + 1;
            cell.neighbors[2] = cell.x - 1;  cell.neighbors[NEIGHBORS + 2] = cell.y;
            cell.neighbors[3] = cell.x - 1;  cell.neighbors[NEIGHBORS + 3] = cell.y - 1;
            cell.neighbors[4] = cell.x;      cell.neighbors[NEIGHBORS + 4] = cell.y - 1;
            return cell;
        }
    }
    if(cell.y == 0){
        if((cell.x != 0) && (cell.x != height-1)){
            cell.count_neighbors = 5;
            cell.neighbors[0] = cell.x - 1;  cell.neighbors[NEIGHBORS + 0] = cell.y;
            cell.neighbors[1] = cell.x - 1;  cell.neighbors[NEIGHBORS + 1] = cell.y + 1;
            cell.neighbors[2] = cell.x;      cell.neighbors[NEIGHBORS + 2] = cell.y + 1;
            cell.neighbors[3] = cell.x + 1;  cell.neighbors[NEIGHBORS + 3] = cell.y + 1;
            cell.neighbors[4] = cell.x + 1;  cell.neighbors[NEIGHBORS + 4] = cell.y;
            return cell;
        }
    }
    if(cell.y == width-1){
        if((cell.x != 0) && (cell.x != height-1)){
            cell.count_neighbors = 5;
            cell.neighbors[0] = cell.x - 1;  cell.neighbors[NEIGHBORS + 0] = cell.y;
            cell.neighbors[1] = cell.x - 1;  cell.neighbors[NEIGHBORS + 1] = cell.y - 1;
            cell.neighbors[2] = cell.x;      cell.neighbors[NEIGHBORS + 2] = cell.y - 1;
            cell.neighbors[3] = cell.x + 1;  cell.neighbors[NEIGHBORS + 3] = cell.y - 1;
            cell.neighbors[4] = cell.x + 1;  cell.neighbors[NEIGHBORS + 4] = cell.y;
            return cell;
        }
    }
    if(cell.x > 0 && cell.x < height-1 && cell.y > 0 && cell.y < width-1){
        cell.count_neighbors = 8;
        cell.neighbors[0] = cell.x - 1;  cell.neighbors[NEIGHBORS + 0] = cell.y;
        cell.neighbors[1] = cell.x - 1;  cell.neighbors[NEIGHBORS + 1] = cell.y - 1;
        cell.neighbors[2] = cell.x;      cell.neighbors[NEIGHBORS + 2] = cell.y - 1;
        cell.neighbors[3] = cell.x + 1;  cell.neighbors[NEIGHBORS + 3] = cell.y - 1;
        cell.neighbors[4] = cell.x + 1;  cell.neighbors[NEIGHBORS + 4] = cell.y;
        cell.neighbors[5] = cell.x + 1;  cell.neighbors[NEIGHBORS + 5] = cell.y + 1;
        cell.neighbors[6] = cell.x;      cell.neighbors[NEIGHBORS + 6] = cell.y + 1;
        cell.neighbors[7] = cell.x - 1;  cell.neighbors[NEIGHBORS + 7] = cell.y + 1;
        return cell;
    }
}

// "enganando o compilador"
template class Cell<double>;
template class Cell<int>;

template<class T>
MPI_Datatype Make_MPI_Cell(){
    MPI_Datatype MPI_Attribute = Make_MPI_Atribute<T>();

    MPI_Datatype MPI_Cell;

    const int nitens = 5;
    MPI_Datatype blocktypes[nitens] = {MPI_INT, MPI_INT, MPI_Attribute, MPI_INT, MPI_INT};
    int blocklengths[nitens] = {1, 1, 1, (NEIGHBORS + NEIGHBORS), 1};
    MPI_Aint offsets[nitens] = {offsetof(Cell<T>, x), offsetof(Cell<T>, y), offsetof(Cell<T>, attribute),
        offsetof(Cell<T>, neighbors), offsetof(Cell<T>, count_neighbors)};

    MPI_Type_create_struct(nitens, blocklengths, offsets, blocktypes, &MPI_Cell);
    MPI_Type_commit(&MPI_Cell);

    return MPI_Cell;
}

// void AddNeighbor(const int &x_neighbor, const int &y_neighbor){
//     this->neighbors[count_neighbors] = x_neighbor;
//     this->neighbors[NEIGHBORS + this->count_neighbors] = y_neighbor;
//     this->count_neighbors = this->count_neighbors + 1;
// }
