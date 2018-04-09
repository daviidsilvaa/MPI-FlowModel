
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
double Model<T>::execute(const MPI_Comm &mpi_comm, CellularSpace<T> *cs){

    switch(cs->type){
        case LINE:
        this->executeLine(mpi_comm, cs);
        break;
        case RECTANGULAR:
        this->executeRectangular(mpi_comm, cs);
        break;
        default:
        return 1;
    }
}

template<class T>
double Model<T>::executeLine(const MPI_Comm &mpi_comm, CellularSpace<T> *cs){
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
            int cs_height = cs->getHeight() * comm_workers, cs_width = cs->getWidth();

            // caso a maquina seja MASTER
            if(comm_rank == MASTER){

                // designa que uma maquina execute um Flow::execute()
                char word_execute_send[23];
                int dest_ = (this->flows[i]->source.x/(cs_height/comm_workers)) + 1;
                sprintf(word_execute_send, "%d|%d:%d|%lf", dest_, this->flows[i]->source.x, this->flows[i]->source.y, this->flows[i]->getFlowRate());
                cout << "rank " << comm_rank << " word_execute_send " << word_execute_send << endl;

                for(int dest = 1; dest <= comm_workers; dest++){
                    MPI_Send(word_execute_send, 23, MPI_CHAR, dest, 999, mpi_comm);
                }
            }
            // caso a maquina seja SLAYER
            if(comm_rank != MASTER){
                char word_execute_recv[23];

                MPI_Recv(word_execute_recv, 23, MPI_CHAR, MASTER, 999, mpi_comm, &mpi_status);
                cout << "rank " << comm_rank << " word_execute_recv " << word_execute_recv << endl;
                char *rank_c = strtok(word_execute_recv, "|:");
                char *x_c = strtok(NULL, ":");
                char *y_c = strtok(NULL, "|");
                char *flow_rate_c = strtok(NULL, "|:");
                int rank_ = atoi(rank_c);
                int x_ = atoi(x_c);
                int y_ = atoi(y_c);
                int flow_rate_ = atoi(flow_rate_c);

                // se a maquina for a que armazena a celula a ser fluxionada, ela executa o fluxo
                if(comm_rank == rank_){

                    this->flows[i]->setLastExecute(this->flows[i]->execute());

                    int count_neighbors_send, y_send;
                    T last_execute_send;

                    // Atualizando o valor do atributo na maquina vizinha
                    if(cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()].getX() - cs->getXInit() == (cs_height/comm_workers)-1){

                        // caso a maquina tenha sucessora
                        if(rank_ == 1){
                            switch(cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()].count_neighbors){
                                case 5:{
                                    Attribute<T> attrib_tmp;
                                    int rank__ = rank_ + 1;

                                    count_neighbors_send = 2;
                                    last_execute_send = this->flows[i]->getLastExecute()/cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()].count_neighbors;

                                    if(cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()].getY() == 0){
                                        cout << __FILE__ << ": " << __LINE__ << endl;
                                        int rank_type = 5;
                                        y_send = cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()].getY();

                                        // incrementando valor nas celulas vizinhas na maquina distinta
                                        for(int dest_ = 1; dest_ <= comm_workers; dest_++){
                                            MPI_Send(&rank__, 1, MPI_INT, dest_, rank_+8, MPI_COMM_WORLD);
                                            MPI_Send(&rank_type, 1, MPI_INT, dest_, rank_+9, MPI_COMM_WORLD);
                                        }

                                        cout << "rank " << comm_rank << " last_execute_send " << last_execute_send << endl;
                                        MPI_Send(&count_neighbors_send, 1, MPI_INT, rank__, rank_, MPI_COMM_WORLD);
                                        MPI_Send(&last_execute_send, 1, ConvertType(getAbstractionDataType<T>()), rank__, rank_, MPI_COMM_WORLD);
                                        MPI_Send(&y_send, 1, MPI_INT, rank__, rank_+10, MPI_COMM_WORLD);

                                        // incrementando valor nas celulas vizinhas na mesma maquina
                                        attrib_tmp = cs->memoria[x_*cs->getWidth() + y_+1 - cs->getXInit()].getAttribute();
                                        attrib_tmp.setValue(attrib_tmp.getValue() + last_execute_send);
                                        cs->memoria[x_*cs->getWidth() + y_+1 - cs->getXInit()].setAttribute(attrib_tmp);

                                        for(int j = 0; j < count_neighbors_send; j++){
                                            attrib_tmp = cs->memoria[(x_-1)*cs->getWidth() + y_+i - cs->getXInit()].getAttribute();
                                            attrib_tmp.setValue(attrib_tmp.getValue() + last_execute_send);
                                            cs->memoria[(x_-1)*cs->getWidth() + y_+i - cs->getXInit()].setAttribute(attrib_tmp);
                                        }
                                    }else{
                                        cout << __FILE__ << ": " << __LINE__ << endl;
                                        int rank_type = 6;
                                        y_send = cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()].getY() - 1;

                                        // incrementando valor nas celulas vizinhas na maquina distinta
                                        for(int dest_ = 1; dest_ <= comm_workers; dest_++){
                                            MPI_Send(&rank__, 1, MPI_INT, dest_, rank_+8, MPI_COMM_WORLD);
                                            MPI_Send(&rank_type, 1, MPI_INT, dest_, rank_+9, MPI_COMM_WORLD);
                                        }

                                        cout << "rank " << comm_rank << " last_execute_send " << last_execute_send << endl;
                                        MPI_Send(&count_neighbors_send, 1, MPI_INT, rank__, rank_, MPI_COMM_WORLD);
                                        MPI_Send(&last_execute_send, 1, ConvertType(getAbstractionDataType<T>()), rank__, rank_, MPI_COMM_WORLD);
                                        MPI_Send(&y_send, 1, MPI_INT, rank__, rank_+10, MPI_COMM_WORLD);

                                        // incrementando valor nas celulas vizinhas na mesma maquina
                                        attrib_tmp = cs->memoria[x_*cs->getWidth() + y_-1 - cs->getXInit()].getAttribute();
                                        attrib_tmp.setValue(attrib_tmp.getValue() + last_execute_send);
                                        cs->memoria[x_*cs->getWidth() + y_-1 - cs->getXInit()].setAttribute(attrib_tmp);

                                        for(int j = 0; j < count_neighbors_send; j++){
                                            attrib_tmp = cs->memoria[(x_-1)*cs->getWidth() + y_-1+i - cs->getXInit()].getAttribute();
                                            attrib_tmp.setValue(attrib_tmp.getValue() + last_execute_send);
                                            cs->memoria[(x_-1)*cs->getWidth() + y_-1+i - cs->getXInit()].setAttribute(attrib_tmp);
                                        }
                                    }

                                    // decremetando valor na celula source
                                    attrib_tmp = (cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()]).getAttribute();
                                    attrib_tmp.setValue(attrib_tmp.getValue() - this->flows[i]->getLastExecute());
                                    cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()].setAttribute(attrib_tmp);
                                }break;
                                case 8:{
                                    Attribute<T> attrib_tmp;
                                    int rank__ = rank_ + 1, rank_type = 8;

                                    count_neighbors_send = 3;
                                    last_execute_send = this->flows[i]->getLastExecute()/cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()].count_neighbors;
                                    y_send = cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()].y - 1;

                                    // incrementando valor nas celulas vizinhas na maquina distinta
                                    for(int dest_ = 1; dest_ <= comm_workers; dest_++){
                                        MPI_Send(&rank__, 1, MPI_INT, dest_, rank_+8, MPI_COMM_WORLD);
                                        MPI_Send(&rank_type, 1, MPI_INT, dest_, rank_+9, MPI_COMM_WORLD);
                                    }

                                    cout << "rank " << comm_rank << " last_execute_send " << last_execute_send << endl;
                                    MPI_Send(&count_neighbors_send, 1, MPI_INT, rank__, rank_, MPI_COMM_WORLD);
                                    MPI_Send(&last_execute_send, 1, ConvertType(getAbstractionDataType<T>()), rank__, rank_, MPI_COMM_WORLD);
                                    MPI_Send(&y_send, 1, MPI_INT, rank__, rank_+10, MPI_COMM_WORLD);

                                    // incrementando valor nas celulas vizinhas na mesma maquina
                                    attrib_tmp = cs->memoria[x_*cs->getWidth() + y_-1 - cs->getXInit()].getAttribute();
                                    attrib_tmp.setValue(attrib_tmp.getValue() + last_execute_send);
                                    cs->memoria[x_*cs->getWidth() + y_-1 - cs->getXInit()].setAttribute(attrib_tmp);

                                    attrib_tmp = cs->memoria[x_*cs->getWidth() + y_+1 - cs->getXInit()].getAttribute();
                                    attrib_tmp.setValue(attrib_tmp.getValue() + last_execute_send);
                                    cs->memoria[x_*cs->getWidth() + y_+1 - cs->getXInit()].setAttribute(attrib_tmp);


                                    for(int j = 0; j < count_neighbors_send; j++){
                                        attrib_tmp = cs->memoria[(x_-1)*cs->getWidth() + y_-1+i - cs->getXInit()].getAttribute();
                                        attrib_tmp.setValue(attrib_tmp.getValue() + last_execute_send);
                                        cs->memoria[(x_-1)*cs->getWidth() + y_-1+i - cs->getXInit()].setAttribute(attrib_tmp);
                                    }

                                    // decremetando valor na celula source
                                    attrib_tmp = (cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()]).getAttribute();
                                    attrib_tmp.setValue(attrib_tmp.getValue() - this->flows[i]->getLastExecute());
                                    cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()].setAttribute(attrib_tmp);

                                }break;
                                default:
                                    cout << __FILE__ << ": " << __LINE__ << endl;
                            }
                        }
                        // caso a maquina NAO tenha sucessora
                        if(rank_ == (comm_size - 1)){
                            cout << __FILE__ << ": " << __LINE__ << endl;
                            int rank__ = -1, rank_type = -1;
                            Attribute<T> attrib_tmp;

                            // envia mensagem invalida, pois na existem fluxo desta maquina para outra
                            for(int dest_ = 1; dest_ <= comm_workers; dest_++){
                                MPI_Send(&rank__, 1, MPI_INT, dest_, rank_+8, MPI_COMM_WORLD);
                                MPI_Send(&rank_type, 1, MPI_INT, dest_, rank_+9, MPI_COMM_WORLD);
                            }

                            switch(cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()].count_neighbors){
                                case 3:{
                                    last_execute_send = this->flows[i]->getLastExecute()/cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()].count_neighbors;

                                    if(cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()].getY() == 0){
                                        // atualizando celulas vizinhas
                                        for(int j = 0; j < 2; j++){
                                            attrib_tmp = cs->memoria[(x_-1)*cs->getWidth() + y_+i - cs->getXInit()].getAttribute();
                                            attrib_tmp.setValue(attrib_tmp.getValue() + last_execute_send);
                                            cs->memoria[(x_-1)*cs->getWidth() + y_+i - cs->getXInit()].setAttribute(attrib_tmp);
                                        }
                                        attrib_tmp = cs->memoria[(x_)*cs->getWidth() + y_+1 - cs->getXInit()].getAttribute();
                                        attrib_tmp.setValue(attrib_tmp.getValue() + last_execute_send);
                                        cs->memoria[(x_)*cs->getWidth() + y_+1 - cs->getXInit()].setAttribute(attrib_tmp);

                                        // atualizando celula source
                                        attrib_tmp = cs->memoria[(x_)*cs->getWidth() + y_ - cs->getXInit()].getAttribute();
                                        attrib_tmp.setValue(attrib_tmp.getValue() - this->flows[i]->getLastExecute());
                                        cs->memoria[(x_)*cs->getWidth() + y_ - cs->getXInit()].setAttribute(attrib_tmp);
                                    } else {
                                        // atualizando celulas vizinhas
                                        for(int j = 0; j < 2; j++){
                                            attrib_tmp = cs->memoria[(x_-1)*cs->getWidth() + y_-1+i - cs->getXInit()].getAttribute();
                                            attrib_tmp.setValue(attrib_tmp.getValue() + last_execute_send);
                                            cs->memoria[(x_-1)*cs->getWidth() + y_-1+i - cs->getXInit()].setAttribute(attrib_tmp);
                                        }
                                        attrib_tmp = cs->memoria[(x_)*cs->getWidth() + y_-1 - cs->getXInit()].getAttribute();
                                        attrib_tmp.setValue(attrib_tmp.getValue() + last_execute_send);
                                        cs->memoria[(x_)*cs->getWidth() + y_-1 - cs->getXInit()].setAttribute(attrib_tmp);

                                        // atualizando celula source
                                        attrib_tmp = cs->memoria[(x_)*cs->getWidth() + y_ - cs->getXInit()].getAttribute();
                                        attrib_tmp.setValue(attrib_tmp.getValue() - this->flows[i]->getLastExecute());
                                        cs->memoria[(x_)*cs->getWidth() + y_ - cs->getXInit()].setAttribute(attrib_tmp);
                                    }
                                }break;
                                case 5:{
                                    last_execute_send = this->flows[i]->getLastExecute()/cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()].count_neighbors;

                                    // atualizando celulas vizinhas
                                    for(int j = 0; j < 3; j++){
                                        attrib_tmp = cs->memoria[(x_-1)*cs->getWidth() + y_-1+i - cs->getXInit()].getAttribute();
                                        attrib_tmp.setValue(attrib_tmp.getValue() + last_execute_send);
                                        cs->memoria[(x_-1)*cs->getWidth() + y_-1+i - cs->getXInit()].setAttribute(attrib_tmp);
                                    }
                                    attrib_tmp = cs->memoria[(x_)*cs->getWidth() + y_-1 - cs->getXInit()].getAttribute();
                                    attrib_tmp.setValue(attrib_tmp.getValue() + last_execute_send);
                                    cs->memoria[(x_)*cs->getWidth() + y_-1 - cs->getXInit()].setAttribute(attrib_tmp);

                                    attrib_tmp = cs->memoria[(x_)*cs->getWidth() + y_+1 - cs->getXInit()].getAttribute();
                                    attrib_tmp.setValue(attrib_tmp.getValue() + last_execute_send);
                                    cs->memoria[(x_)*cs->getWidth() + y_+1 - cs->getXInit()].setAttribute(attrib_tmp);

                                    // atualizando celula source
                                    attrib_tmp = cs->memoria[(x_)*cs->getWidth() + y_ - cs->getXInit()].getAttribute();
                                    attrib_tmp.setValue(attrib_tmp.getValue() - this->flows[i]->getLastExecute());
                                    cs->memoria[(x_)*cs->getWidth() + y_ - cs->getXInit()].setAttribute(attrib_tmp);
                                }break;
                                default:
                                    cout << __FILE__ << ": " << __LINE__ << endl;
                            }
                        }
                    }

                    if(cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()].x - cs->getXInit() == PROC_DIMX){
                        cout << __FILE__ << ": " << __LINE__ << endl;
                    }
                }
                if(comm_rank != rank_){
                    int rank__, rank_type;

                    cout << comm_rank << " " << __FILE__ << ": " << __LINE__ << endl;
                    MPI_Recv(&rank__, 1, MPI_INT, rank_, rank_+8, MPI_COMM_WORLD, &mpi_status);
                    MPI_Recv(&rank_type, 1, MPI_INT, rank_, rank_+9, MPI_COMM_WORLD, &mpi_status);
                    cout << comm_rank << " " << __FILE__ << ": " << __LINE__ << endl;

                    if(comm_rank == rank__){
                        int count_neighbors_recv, y_recv;
                        T last_execute_recv;
                        Attribute<T> attrib_tmp;

                        MPI_Recv(&count_neighbors_recv, 1, MPI_INT, rank_, rank_, MPI_COMM_WORLD, &mpi_status);
                        MPI_Recv(&last_execute_recv, 1, ConvertType(getAbstractionDataType<T>()), rank_, rank_, MPI_COMM_WORLD, &mpi_status);
                        MPI_Recv(&y_recv, 1, MPI_INT, rank_, rank_+10, MPI_COMM_WORLD, &mpi_status);

                        cout << "rank " << comm_rank << " last_execute_recv " << last_execute_recv << endl;
                        for(int i = 0; i < count_neighbors_recv; i++){
                            attrib_tmp = cs->memoria[y_recv + i].getAttribute();
                            attrib_tmp.setValue(attrib_tmp.getValue() + last_execute_recv);
                            cs->memoria[y_recv + i].setAttribute(attrib_tmp);
                        }
                    }
                }
            }
        }
    }
}

template<class T>
double Model<T>::executeRectangular(const MPI_Comm &mpi_comm, CellularSpace<T> *cs){
    return 0.5;
}


template class Model<double>;
