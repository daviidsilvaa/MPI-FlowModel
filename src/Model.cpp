
#include "Model.hpp"

#include <iostream>
using namespace std;

template<class T>
Model<T>::Model(){ }

template<class T>
Model<T>::Model(const double &time, const double &time_step){
    this->time = time;
    this->time_step = time_step;
}

template<class T>
Model<T>::Model(const Model<T> &model){
    this->flows = model.flows;
    this->time = model.time;
    this->time_step = model.time_step;
}

template<class T>
Model<T>& Model<T>::operator=(const Model<T> &model){
    if(this != &model){
        this->flows = model.flows;
        this->time = model.time;
        this->time_step = model.time_step;
    }
    return *this;
}

template<class T>
Model<T>::~Model(){ }

template<class T>
void Model<T>::addFlow(FlowImpl<T> *flow){
    this->flows.push_back(flow);
}

template<class T>
double Model<T>::execute(const MPI_Comm &mpi_comm, const CellularSpace<T> &cellular_space){

    switch(cellular_space.getType()){
        case LINE:
            this->executeLine(mpi_comm, cellular_space);
            break;
        case RECTANGULAR:
            this->executeRectangular(mpi_comm, cellular_space);
            break;
        default:
            return 1;
    }
}

template<class T>
double Model<T>::executeLine(const MPI_Comm &mpi_comm, const CellularSpace<T> &cellular_space){
    int comm_size, comm_rank, comm_workers;
    MPI_Comm_size(mpi_comm, &comm_size);
    MPI_Comm_rank(mpi_comm, &comm_rank);
    MPI_Status mpi_status;
    MPI_Request mpi_send_request, mpi_recv_request;

    comm_workers = comm_size - 1;

    // for para tempo de execucao
    for(double t = 0; t < this->time; t = t + this->time_step){

        // for para executar cada fluxo
        for(int i = 0; i < this->flows.size(); i++){
            int cs_height = cellular_space.getHeight() * comm_workers, cs_width = cellular_space.getWidth();

            // caso a maquina seja MASTER
            if(comm_rank == MASTER){

                // designa que uma maquina execute um Flow::execute()
                char word_execute_send[23];
                int dest_ = (this->flows[i]->source.x/(cs_height/comm_workers)) + 1;
                sprintf(word_execute_send, "%d|%d:%d|%lf", dest_, this->flows[i]->source.x, this->flows[i]->source.y, this->flows[i]->flow_rate);
                cout << comm_rank << "\t" << word_execute_send << endl;

                for(int dest = 1; dest <= comm_workers; dest++){
                    MPI_Send(word_execute_send, 23, MPI_CHAR, dest, 999, mpi_comm);
                }
            }
            // caso a maquina seja SLAYER
            if(comm_rank != MASTER){
                char word_execute_recv[23];


                MPI_Recv(word_execute_recv, 23, MPI_CHAR, MASTER, 999, mpi_comm, &mpi_status);
                char *rank_c = strtok(word_execute_recv, "|:");
                char *x_c = strtok(NULL, ":");
                char *y_c = strtok(NULL, "|");
                char *flow_rate_c = strtok(NULL, "|:");
                int rank_ = atoi(rank_c);
                int x_ = atoi(x_c);
                int y_ = atoi(y_c);
                int flow_rate_ = atoi(flow_rate_c);

                if(comm_rank == rank_){
                    cout << comm_rank << "\t" << endl;
                }
            }
        }
    }
}

template<class T>
double Model<T>::executeRectangular(const MPI_Comm &mpi_comm, const CellularSpace<T> &cellular_space){
    return 0.5;
}


template class Model<double>;
