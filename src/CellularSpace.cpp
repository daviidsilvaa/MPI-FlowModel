
#include "CellularSpace.hpp"
#include "mpi.h"

template<class T>
CellularSpace<T>::CellularSpace(){ }

template<class T>
CellularSpace<T>::CellularSpace(const int &height, const int &width){
    this->x_init = 0;
    this->y_init = 0;
    this->width = width;
    this->height = height;
}

template<class T>
CellularSpace<T>::CellularSpace(const int &x_init, const int &y_init, const int &height, const int &width){
    this->x_init = x_init;
    this->y_init = y_init;
    this->width = width;
    this->height = height;
}

template<class T>
CellularSpace<T>::~CellularSpace(){ }

template<class T>
void CellularSpace<T>::setXInit(const int &x_init){
    this->x_init = x_init;
}
template<class T>
int CellularSpace<T>::getXInit() const{
    return this->x_init;
}

template<class T>
void CellularSpace<T>::setYInit(const int &x_init){
    this->y_init = y_init;
}
template<class T>
int CellularSpace<T>::getYInit() const{
    return this->y_init;
}

template<class T>
void CellularSpace<T>::setHeight(const int &height){
    this->height = height;
}
template<class T>
int CellularSpace<T>::getHeight() const{
    return this->height;
}

template<class T>
void CellularSpace<T>::setWidth(const int &width){
    this->width = width;
}
template<class T>
int CellularSpace<T>::getWidth() const{
    return this->width;
}

template<class T>
void CellularSpace<T>::setType(const int &type){
    this->type = type;
}
template<class T>
int CellularSpace<T>::getType() const{
    return this->type;
}

template<class T>
void CellularSpace<T>::LineScatter(const MPI_Comm &mpi_comm){
    // seta o tipo do espaco celular

    int comm_size, comm_rank, comm_workers, count, offset;

    MPI_Comm_size(mpi_comm, &comm_size);
    MPI_Comm_rank(mpi_comm, &comm_rank);
    MPI_Status mpi_status;
    MPI_Request mpi_send_request, mpi_recv_request;

    comm_workers = comm_size - 1;

    if(comm_rank == 0){
        count = (this->getHeight()*this->getWidth())/comm_workers;
        offset = 0;
        int x_init_s, y_init_s, height_s, width_s;
        int index[comm_size];
        char word_cs_send[23];

        // para cada maquina { crie uma regiao (i.e.linhas) do espaço celular }
        for(int dest = 1; dest <= comm_size-1; dest++){
            sprintf(word_cs_send, "%d|%d:%d|%d",
            offset/this->getWidth(), this->getYInit(), this->getHeight()/comm_workers, this->getWidth());
            MPI_Send(word_cs_send, 23, MPI_CHAR, dest, FROM_MASTER, MPI_COMM_WORLD);
            offset = offset + count;
            index[dest]= offset;
        }
    } else {
        char word_cs_recv[23], word_execute_recv[23];

        MPI_Recv(word_cs_recv, 23, MPI_CHAR, MASTER, FROM_MASTER, MPI_COMM_WORLD, &mpi_status);
        char *x_init_c = strtok(word_cs_recv, "|");
        char *y_init_c = strtok(NULL, ":");
        char *height_c = strtok(NULL, "|");
        char *width_c = strtok(NULL, ":");
        int x_init_s = atoi(x_init_c);
        int y_init_s = atoi(y_init_c);
        int height_s = atoi(height_c);
        int width_s = atoi(width_c);

        // inicializa o espaco celular em cada maquina
        *this = CellularSpace<T>(x_init_s, y_init_s, height_s, width_s);

        // cria as celulas dentro de cada espaco celular
        for(int i = 0; i < (this->getHeight() * this->getWidth()); i++){
            this->memoria[i] = Cell<T>((this->getXInit() + (i/this->getWidth())), (i%this->getWidth()), Attribute<T>(i, 1));
            this->memoria[i] = this->memoria[i].setNeighbor();
        }
    }

    this->setType(LINE);
}

// "enganando o compilador"
template class CellularSpace<double>;
template class CellularSpace<int>;