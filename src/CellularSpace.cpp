
#include "CellularSpace.hpp"
#include "mpi.h"
#include <cmath>

template<class T>
CellularSpace<T>::CellularSpace(){ }

template<class T>
CellularSpace<T>::CellularSpace(const int &height, const int &width){
    this->x_init = 0;
    this->y_init = 0;
    this->width = width;
    this->height = height;
    this->memoria = std::vector<Cell<T> >(height*width);
}

template<class T>
CellularSpace<T>::CellularSpace(const int &x_init, const int &y_init, const int &height, const int &width){
    this->x_init = x_init;
    this->y_init = y_init;
    this->width = width;
    this->height = height;
    this->memoria = std::vector<Cell<T> >(height*width);
}

template<class T>
CellularSpace<T>::~CellularSpace(){ }

template<class T>
void CellularSpace<T>::setXInit(const int &x_init){
    this->x_init = x_init;
}
template<class T>
int CellularSpace<T>::getXInit(){
    return this->x_init;
}

template<class T>
void CellularSpace<T>::setYInit(const int &x_init){
    this->y_init = y_init;
}
template<class T>
int CellularSpace<T>::getYInit(){
    return this->y_init;
}

template<class T>
void CellularSpace<T>::setHeight(const int &height){
    this->height = height;
}
template<class T>
int CellularSpace<T>::getHeight(){
    return this->height;
}

template<class T>
void CellularSpace<T>::setWidth(const int &width){
    this->width = width;
}
template<class T>
int CellularSpace<T>::getWidth(){
    return this->width;
}

template<class T>
int CellularSpace<T>::getSize(){
    return this->memoria.size();
}

template<class T>
void CellularSpace<T>::setType(const int &type){
    this->type = type;
}
template<class T>
int CellularSpace<T>::getType(){
    return this->type;
}

template<class T>
void CellularSpace<T>::scatter(const MPI_Comm &mpi_comm, const int &type){
    switch(type){
        case LINE:{
            this->lineScatter(mpi_comm);
        }break;
        case RECTANGULAR:{
            this->rectangularScatter(mpi_comm);
        }break;
        default:{
            this->setType(-1);
        }
    }
}

template<class T>
void CellularSpace<T>::lineScatter(const MPI_Comm &mpi_comm){
    int comm_size, comm_rank, comm_workers, offset;

    MPI_Comm_size(mpi_comm, &comm_size);
    MPI_Comm_rank(mpi_comm, &comm_rank);
    MPI_Status mpi_status;
    MPI_Request mpi_send_request, mpi_recv_request;

    comm_workers = comm_size - 1;

    if(comm_rank == 0){
        offset = 0;
        int x_init_s, y_init_s, height_s, width_s;
        int index[comm_size];
        char word_cs_send[23];

        // para cada maquina { crie uma regiao (i.e.linhas) do espaço celular }
        for(int dest = 1; dest <= comm_workers; dest++){
            sprintf(word_cs_send, "%d|%d:%d|%d",
            offset, this->getYInit(), this->getHeight(), this->getWidth());
            MPI_Send(word_cs_send, 23, MPI_CHAR, dest, FROM_MASTER, MPI_COMM_WORLD);
            offset = offset + this->getHeight();
            index[dest]= offset;
        }

        // this->height = this->width = 0;
        this->memoria = std::vector<Cell<T> >(0);
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
            this->memoria[i] = this->memoria[i].setNeighbor(height_s*(comm_size-1), width_s);
        }
    }

    this->setType(LINE);
}

template<class T>
void CellularSpace<T>::rectangularScatter(const MPI_Comm &mpi_comm){
    int comm_size, comm_rank, comm_workers;

    MPI_Comm_size(mpi_comm, &comm_size);
    MPI_Comm_rank(mpi_comm, &comm_rank);
    MPI_Status mpi_status, mpi_status_consumer;
    MPI_Request mpi_consumer_request, mpi_recv_request;

    comm_workers = comm_size - 1;

    int cs_height = this->getHeight() * sqrt(comm_workers);
    int cs_width = this->getWidth() * sqrt(comm_workers);

    // maquina master
    if(comm_rank == 0){
        int offset_x = 0, offset_y = 0;
        int x_init_s, y_init_s, height_s, width_s;
        char word_cs_send[23];

        // para cada maquina { crie uma regiao (i.e. retangulos) do espaço celular }
        for(int dest = 1; dest <= comm_workers; dest++){
            sprintf(word_cs_send, "%d|%d:%d|%d",
                offset_x, offset_y, this->getHeight(), this->getWidth());
            MPI_Send(word_cs_send, 23, MPI_CHAR, dest, FROM_MASTER, MPI_COMM_WORLD);
            // std::cout << dest << " " << word_cs_send << std::endl;

            offset_y = offset_y + this->getWidth();
            // verifica se a proxima linha de retangulos sera passada para sua maquina correspondente
            if(offset_y == cs_width){
                offset_x = offset_x + this->getHeight();
                offset_y = 0;
            }
        }

        this->memoria = std::vector<Cell<T> >(0);
    } else {
        // executa a simulacao nas maquinas slayers
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

        *this = CellularSpace<T>(x_init_s, y_init_s, height_s, width_s);

        // std::cout << comm_rank << " " << this->getXInit() << " " << this->getYInit() << " " << this->getHeight() << " " << this->getWidth() << std::endl;

        // inicializando a vizinhanca do cellular space criado em cada maquina
        for(int i = 0; i < (this->getHeight() * this->getWidth()); i++){
            this->memoria[i] = Cell<T>((this->getXInit() + (i/this->getWidth())), ((i%this->getWidth()) + this->getYInit()), Attribute<T>(i, 1));
            this->memoria[i] = this->memoria[i].setNeighbor(cs_height, cs_width);
            // std::cout << (this->getXInit() + (i/this->getWidth())) << "\t" << ((i%this->getWidth()) + this->getYInit()) << "\t" << this->memoria[i].count_neighbors << std::endl;
        }
    }
    this->setType(RECTANGULAR);
}


template class CellularSpace<double>;
template class CellularSpace<int>;
