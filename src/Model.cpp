
#include "Model.hpp"
#include "Time.hpp"

#include <iostream>
#include <assert.h>
using namespace std;

#define ASSERT_VALUE 150

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
void Model<T>::eraseFlows(){
    this->flows.erase(this->flows.begin(), this->flows.end());
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
    int cs_height = cs->getHeight() * comm_workers, cs_width = cs->getWidth();

    double service_time = 0;
    double wait_time = 0;

    // for para tempo de execucao
    for(double t = 0; t < this->time; t = t + this->time_step){

        // for para executar cada fluxo
        for(int i = 0; i < this->flows.size(); i++){
            Time t1 = Time();
            t1.getInit();
            // caso a maquina seja MASTER
            if(comm_rank == MASTER){

                // designa que uma maquina execute um Flow::execute()
                char word_execute_send[23];
                int dest_ = (this->flows[i]->source.x/(cs_height/comm_workers)) + 1;
                sprintf(word_execute_send, "%d|%d:%d|%lf", dest_, this->flows[i]->source.x, this->flows[i]->source.y, this->flows[i]->getFlowRate());
                // cout << "rank " << comm_rank << " word_execute_send " << word_execute_send << endl;
                // cout << i << "\t" << word_execute_send << endl;

                for(int dest = 1; dest <= comm_workers; dest++){
                    MPI_Send(word_execute_send, 23, MPI_CHAR, dest, 3000+i, mpi_comm);
                }

                // de quem a MASTER devera' esperar a confirmacao
                int rank__;
                MPI_Recv(&rank__, 1, MPI_INT, dest_, 1000+i, mpi_comm, &mpi_status);

                char word_barrier;
                // recebe a confirmacao de final de execucao
                MPI_Recv(&word_barrier, 1, MPI_CHAR, dest_, 2000+i, mpi_comm, &mpi_status);
                MPI_Recv(&word_barrier, 1, MPI_CHAR, rank__, 4000+i, mpi_comm, &mpi_status);
                // cout << "rank " << dest_ << ", " << rank__ << " did execute " << i << endl;


            } else { // caso a maquina seja SLAYER
                char word_execute_recv[23];

                MPI_Recv(word_execute_recv, 23, MPI_CHAR, MASTER, 3000+i, mpi_comm, &mpi_status);
                t1.getFinish();

                wait_time += t1.getTotalTime(); // incrementa tempo de espera

                // cout << "rank " << comm_rank << " word_execute_recv " << word_execute_recv << endl;
                char *rank_c = strtok(word_execute_recv, "|:");
                char *x_c = strtok(NULL, ":");
                char *y_c = strtok(NULL, "|");
                char *flow_rate_c = strtok(NULL, "|:");
                int rank_ = atoi(rank_c);
                int x_ = atoi(x_c);
                int y_ = atoi(y_c);
                int flow_rate_ = atoi(flow_rate_c);

                // se a maquina for a que armazena a celula a ser fluxionada, ela executa o fluxo
                Time t2 = Time();
                if(comm_rank == rank_){

                    this->flows[i]->setLastExecute(this->flows[i]->execute());

                    int count_neighbors_send, y_send;
                    T last_execute_send;

                    // Atualizando o valor do atributo na maquina vizinha

                    // caso a Cell esteja na ultima linha da maquina
                    if(cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()].getX() - cs->getXInit() == (cs_height/comm_workers)-1){

                        // caso a maquina tenha sucessora, ie, a Cell nao esta na ultima linha do CellularSpace como um todo
                        //  obs: rank == comm_size-1 significa ser a ultima maquina
                        if(rank_ != (comm_size-1)){
                            switch(cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()].count_neighbors){
                                // caso a Cell esteja no extremo leste ou oeste do CellularSpace
                                case 5:{
                                    t2.getInit(); // calcula tempo de servico

                                    Attribute<T> attrib_tmp;
                                    int rank__ = rank_ + 1;

                                    count_neighbors_send = 2;
                                    last_execute_send = this->flows[i]->getLastExecute()/cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()].count_neighbors;

                                    // caso a celula source esteja na extremidade oeste
                                    if(cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()].getY() == 0){
                                        // cout << __FILE__ << ": " << __LINE__ << endl;
                                        y_send = cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()].getY();

                                        char word_target_send[23];
                                        sprintf(word_target_send, "%d|%d|%lf|%d", rank__, count_neighbors_send,
                                        last_execute_send, y_send);
                                        // cout << word_target_send << endl;

                                        // fala para a MASTER quem ele deve aguardar a confirmacao de espera,
                                        // ie, quem ele deve esperar para sincronizar
                                        MPI_Send(&rank__, 1, MPI_INT, MASTER, 1000+i, MPI_COMM_WORLD);

                                        // incrementando valor nas celulas vizinhas na maquina distinta
                                        for(int dest_ = 1; dest_ <= comm_workers; dest_++){
                                            MPI_Send(&word_target_send, 23, MPI_CHAR, dest_, i, MPI_COMM_WORLD);
                                        }

                                        // incrementando valor nas celulas vizinhas na mesma maquina
                                        attrib_tmp = cs->memoria[x_*cs->getWidth() + y_+1 - cs->getXInit()].getAttribute();
                                        attrib_tmp.setValue(attrib_tmp.getValue() + last_execute_send);
                                        cs->memoria[x_*cs->getWidth() + y_+1 - cs->getXInit()].setAttribute(attrib_tmp);

                                        for(int j = 0; j < count_neighbors_send; j++){
                                            attrib_tmp = cs->memoria[(x_-1)*cs->getWidth() + y_+j - cs->getXInit()].getAttribute();
                                            attrib_tmp.setValue(attrib_tmp.getValue() + last_execute_send);
                                            cs->memoria[(x_-1)*cs->getWidth() + y_+j - cs->getXInit()].setAttribute(attrib_tmp);
                                        }
                                    }else{
                                        // cout << __FILE__ << ": " << __LINE__ << endl;
                                        y_send = cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()].getY() - 1;

                                        char word_target_send[23];
                                        sprintf(word_target_send, "%d|%d|%lf|%d", rank__, count_neighbors_send,
                                        last_execute_send, y_send);

                                        MPI_Send(&rank__, 1, MPI_INT, MASTER, 1000+i, MPI_COMM_WORLD);

                                        // incrementando valor nas celulas vizinhas na maquina distinta
                                        for(int dest_ = 1; dest_ <= comm_workers; dest_++){
                                            MPI_Send(&word_target_send, 23, MPI_CHAR, dest_, i, MPI_COMM_WORLD);
                                        }

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

                                    t2.getFinish();
                                    service_time += t2.getTotalTime();
                                }break; // fim case 5

                                // caso a Cell nao esteja no extremo leste ou oeste do CellularSpace
                                case 8:{
                                    t2.getInit();
                                    Attribute<T> attrib_tmp;
                                    int rank__ = rank_ + 1;

                                    count_neighbors_send = 3;
                                    last_execute_send = this->flows[i]->getLastExecute()/cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()].count_neighbors;
                                    y_send = cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()].y - 1;

                                    char word_target_send[23];
                                    sprintf(word_target_send, "%d|%d|%lf|%d", rank__, count_neighbors_send,
                                    last_execute_send, y_send);

                                    MPI_Send(&rank__, 1, MPI_INT, MASTER, 1000+i, MPI_COMM_WORLD);

                                    // envia palavra "fake" com count_neighbors_send = 0, ou seja, nao havera execucao de fluxo fora daqui
                                    for(int dest_ = 1; dest_ <= comm_workers; dest_++){
                                        MPI_Send(&word_target_send, 23, MPI_CHAR, dest_, i, MPI_COMM_WORLD);
                                    }

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

                                    t2.getFinish();
                                    service_time += t2.getTotalTime();
                                }break; // fim case 8
                                default:
                                // provavel que tenha !erro!
                                cout << __FILE__ << ": " << __LINE__ << endl;
                            } // fim switch

                            char word_barrier = 't';
                            MPI_Send(&word_barrier, 1, MPI_CHAR, MASTER, 2000+i, MPI_COMM_WORLD);
                        } // fim if(rank != comm_workers)
                    } else { // caso Cell source nao esteja na ultima linha do CellularSpace armazenado na maquina
                        //     neste caso, nao e' necessario que outra maquina execute fluxo

                        if(cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()].count_neighbors == 0){
                        	cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()].count_neighbors = 8;
                        }

                        switch (cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()].count_neighbors){
                            case 3:{
                                t2.getInit();
                                Attribute<T> attrib_tmp;

                                char word_target_send[23];

								int rank__ = rank_; // enviara' mensagem para a propria maquina
                                count_neighbors_send = 0;
                                last_execute_send = 0;

                                MPI_Send(&rank__, 1, MPI_INT, MASTER, 1000+i, MPI_COMM_WORLD);

                                sprintf(word_target_send, "%d|%d|%lf|%d", rank__, count_neighbors_send,
                                    last_execute_send, y_send);

                                last_execute_send = this->flows[i]->getLastExecute()/cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()].count_neighbors;

                                // envia palavra "fake" com count_neighbors_send = 0, ou seja, nao havera execucao de fluxo fora daqui
                                for(int dest_ = 1; dest_ <= comm_workers; dest_++){
                                    MPI_Send(&word_target_send, 23, MPI_CHAR, dest_, i, MPI_COMM_WORLD);
                                }

                                // incrementando valor nas celulas vizinhas na mesma maquina
                                if(cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()].x == 0){
                                    attrib_tmp = cs->memoria[(x_)*cs->getWidth() + y_+1 - cs->getXInit()].getAttribute();
                                    attrib_tmp.setValue(attrib_tmp.getValue() + last_execute_send);
                                    cs->memoria[(x_)*cs->getWidth() + y_+1 - cs->getXInit()].setAttribute(attrib_tmp);

                                    attrib_tmp = cs->memoria[(x_+1)*cs->getWidth() + y_ - cs->getXInit()].getAttribute();
                                    attrib_tmp.setValue(attrib_tmp.getValue() + last_execute_send);
                                    cs->memoria[(x_+1)*cs->getWidth() + y_ - cs->getXInit()].setAttribute(attrib_tmp);

                                    attrib_tmp = cs->memoria[(x_+1)*cs->getWidth() + y_+1 - cs->getXInit()].getAttribute();
                                    attrib_tmp.setValue(attrib_tmp.getValue() + last_execute_send);
                                    cs->memoria[(x_+1)*cs->getWidth() + y_+1 - cs->getXInit()].setAttribute(attrib_tmp);
                                }else{
                                    attrib_tmp = cs->memoria[(x_)*cs->getWidth() + y_-1 - cs->getXInit()].getAttribute();
                                    attrib_tmp.setValue(attrib_tmp.getValue() + last_execute_send);
                                    cs->memoria[(x_)*cs->getWidth() + y_-1 - cs->getXInit()].setAttribute(attrib_tmp);

                                    attrib_tmp = cs->memoria[(x_+1)*cs->getWidth() + y_ - cs->getXInit()].getAttribute();
                                    attrib_tmp.setValue(attrib_tmp.getValue() + last_execute_send);
                                    cs->memoria[(x_+1)*cs->getWidth() + y_ - cs->getXInit()].setAttribute(attrib_tmp);

                                    attrib_tmp = cs->memoria[(x_+1)*cs->getWidth() + y_-1 - cs->getXInit()].getAttribute();
                                    attrib_tmp.setValue(attrib_tmp.getValue() + last_execute_send);
                                    cs->memoria[(x_+1)*cs->getWidth() + y_-1 - cs->getXInit()].setAttribute(attrib_tmp);
                                }

                                // decremetando valor na celula source
                                attrib_tmp = (cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()]).getAttribute();
                                attrib_tmp.setValue(attrib_tmp.getValue() - this->flows[i]->getLastExecute());
                                cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()].setAttribute(attrib_tmp);

                                t2.getFinish();
                                service_time += t2.getTotalTime();
                            } break; // fim case 3

                            case 5:{
                                t2.getInit();
                                Attribute<T> attrib_tmp;

                                char word_target_send[23];

                                int rank__ = rank_;

                                count_neighbors_send = 0;
                                last_execute_send = 0;
                                sprintf(word_target_send, "%d|%d|%lf|%d", rank__, count_neighbors_send,
                                    last_execute_send, y_send);

                                last_execute_send = this->flows[i]->getLastExecute()/cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()].count_neighbors;

                                MPI_Send(&rank__, 1, MPI_INT, MASTER, 1000+i, MPI_COMM_WORLD);

                                // envia palavra "fake" com count_neighbors_send = 0, ou seja, nao havera execucao de fluxo fora daqui
                                for(int dest_ = 1; dest_ <= comm_workers; dest_++){
                                    MPI_Send(&word_target_send, 23, MPI_CHAR, dest_, i, MPI_COMM_WORLD);
                                }

                                count_neighbors_send = 3;

                                // incrementando valor nas celulas vizinhas na mesma maquina

                                if(cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()].x == 0){
                                	for(int j = 0; j < count_neighbors_send; j++){
                                    	for(int l = 0; l < (count_neighbors_send-1); l++){
		                                    if(!(j == 1 && l == 0)){
		                                        attrib_tmp = cs->memoria[(x_-1+j)*cs->getWidth() + y_+l - cs->getXInit()].getAttribute();
		                                        attrib_tmp.setValue(attrib_tmp.getValue() + last_execute_send);
		                                        cs->memoria[(x_-1+j)*cs->getWidth() + y_+l - cs->getXInit()].setAttribute(attrib_tmp);
                                        	}
                                    	}
                                	}
                                } else {
                                	for(int j = 0; j < count_neighbors_send; j++){
                                    	for(int l = 0; l < (count_neighbors_send-1); l++){
		                                    if(!(j == 1 && l == 1)){
		                                        attrib_tmp = cs->memoria[(x_-1+j)*cs->getWidth() + y_-1+l - cs->getXInit()].getAttribute();
		                                        attrib_tmp.setValue(attrib_tmp.getValue() + last_execute_send);
		                                        cs->memoria[(x_-1+j)*cs->getWidth() + y_-1+l - cs->getXInit()].setAttribute(attrib_tmp);
		                                    }
		                                }
		                            }
                                }


                                // decremetando valor na celula source
                                attrib_tmp = (cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()]).getAttribute();
                                attrib_tmp.setValue(attrib_tmp.getValue() - this->flows[i]->getLastExecute());
                                cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()].setAttribute(attrib_tmp);

                                t2.getFinish();
                                service_time += t2.getTotalTime();
                            } break; // fim case 5

                            case 8:{
                                t2.getInit();
                                Attribute<T> attrib_tmp;

                                char word_target_send[23];

                                int rank__ = rank_;

                                count_neighbors_send = 0;
                                last_execute_send = 0;
                                sprintf(word_target_send, "%d|%d|%lf|%d", rank__, count_neighbors_send,
                                    last_execute_send, y_send);

                                last_execute_send = this->flows[i]->getLastExecute()/cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()].count_neighbors;

                                MPI_Send(&rank__, 1, MPI_INT, MASTER, 1000+i, MPI_COMM_WORLD);

                                // envia palavra "fake" com count_neighbors_send = 0, ou seja, nao havera execucao de fluxo fora daqui
                                for(int dest_ = 1; dest_ <= comm_workers; dest_++){
                                    MPI_Send(&word_target_send, 23, MPI_CHAR, dest_, i, MPI_COMM_WORLD);
                                }

                                count_neighbors_send = 3;

                                // incrementando valor nas celulas vizinhas na mesma maquina
                                for(int j = 0; j < count_neighbors_send; j++){
                                    for(int l = 0; l < count_neighbors_send; l++){
                                        if(!(j == 1 && l == 1)){
                                            attrib_tmp = cs->memoria[(x_-1+j)*cs->getWidth() + y_-1+l - cs->getXInit()].getAttribute();
                                            attrib_tmp.setValue(attrib_tmp.getValue() + last_execute_send);
                                            cs->memoria[(x_-1+j)*cs->getWidth() + y_-1+l - cs->getXInit()].setAttribute(attrib_tmp);
                                        }
                                    }
                                }

                                // decremetando valor na celula source
                                attrib_tmp = (cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()]).getAttribute();
                                attrib_tmp.setValue(attrib_tmp.getValue() - this->flows[i]->getLastExecute());
                                cs->memoria[x_*cs->getWidth() + y_ - cs->getXInit()].setAttribute(attrib_tmp);

                                t2.getFinish();
                                service_time += t2.getTotalTime();
                            } break; // fim case 8

                            default:{
                                t2.getInit();
                            	// cout << i << " " << __FILE__ << " LINE " << __LINE__ << endl;

                            	char word_target_send[23];
                            	int rank__ = rank_;

                                count_neighbors_send = 0;
                                last_execute_send = 0;
                                sprintf(word_target_send, "%d|%d|%lf|%d", rank__, count_neighbors_send,
                                    last_execute_send, y_send);

                                MPI_Send(&rank__, 1, MPI_INT, MASTER, 1000+i, MPI_COMM_WORLD);

                                // envia palavra "fake" com count_neighbors_send = 0, ou seja, nao havera execucao de fluxo fora daqui
                                for(int dest_ = 1; dest_ <= comm_workers; dest_++){
                                    MPI_Send(&word_target_send, 23, MPI_CHAR, dest_, i, MPI_COMM_WORLD);
                                }

                                t2.getFinish();
                                service_time += t2.getTotalTime();
							} break; // fim default
                        } // fim switch

                        char word_barrier = 't';
                        MPI_Send(&word_barrier, 1, MPI_CHAR, MASTER, 2000+i, MPI_COMM_WORLD);
                    }
                }

                Time t3 = Time();
                t3.getInit();
                int rank__;
                int count_neighbors_recv, y_recv;
                T last_execute_recv;
                Attribute<T> attrib_tmp;

                char word_target_recv[23]; // word_target_recv = rank|count|last_execute|y

                MPI_Recv(word_target_recv, 23, MPI_CHAR, rank_, i, MPI_COMM_WORLD, &mpi_status);
                t3.getFinish();
                wait_time += t3.getTotalTime();

                // cout << i << " rank " << comm_rank << " word_execute_recv " << word_execute_recv << endl;
                rank_c = strtok(word_target_recv, "|");
                char *count_c = strtok(NULL, "|");
                char *last_execute_c = strtok(NULL, "|");
                y_c = strtok(NULL, "|");

                rank__ = atoi(rank_c);
                count_neighbors_recv = atoi(count_c);
                last_execute_recv = atof(last_execute_c);
                y_recv = atoi(y_c);

                // cout << " - " << comm_rank << endl;

                if(comm_rank == rank__){
                    t2.getInit();
                    // cout << "rank " << comm_rank << " last_execute_recv " << last_execute_recv << endl;
                    for(int j = 0; j < count_neighbors_recv; j++){
                        attrib_tmp = cs->memoria[y_recv + j].getAttribute();
                        attrib_tmp.setValue(attrib_tmp.getValue() + last_execute_recv);
                        cs->memoria[y_recv + j].setAttribute(attrib_tmp);
                    }

                    // envia confirmacao de final de execucao
                    char word_barrier = 't';
                    MPI_Send(&word_barrier, 1, MPI_CHAR, MASTER, 4000+i, MPI_COMM_WORLD);

                    t2.getFinish();
                    service_time += t2.getTotalTime();
                }
            }
        }
    }

    // calcula a media do tempo de servico e espera entre as maquinas
    if(comm_rank == MASTER){
        double service_wait_time_average[comm_workers][2];

        for(int source = 1; source <= comm_workers; source++){
            MPI_Recv(service_wait_time_average[source-1], 2, MPI_DOUBLE, source, 8000, mpi_comm, &mpi_status);
        }

        double service_time_average = 0;
        double wait_time_average = 0;

        for(int i = 0; i < comm_workers; i++){
            service_time_average += service_wait_time_average[i][0];
            wait_time_average += service_wait_time_average[i][1];
        }

        cout << "s_time " << service_time_average << endl;
        cout << "w_time " << wait_time_average << endl;

    } else {
        double ser_wai_t_avg[2] = {service_time, wait_time};

        MPI_Send(ser_wai_t_avg, 2, MPI_DOUBLE, MASTER, 8000, MPI_COMM_WORLD);
    }
}

template<class T>
double Model<T>::executeRectangular(const MPI_Comm &mpi_comm, CellularSpace<T> *cs){
    int comm_size, comm_rank, comm_workers;
    MPI_Comm_size(mpi_comm, &comm_size);
    MPI_Comm_rank(mpi_comm, &comm_rank);
    MPI_Status mpi_status;
    MPI_Request mpi_send_request, mpi_recv_request;

    comm_workers = comm_size - 1;

    double service_time = 0;
    double wait_time = 0;

    // for para tempo de execucao
    for(double t = 0; t < this->time; t = t + this->time_step){
        Time t1 = Time();
        t1.getInit();
        // for para executar cada fluxo
        for(int i = 0; i < this->flows.size(); i++){
            int cs_height = cs->getHeight() * comm_workers, cs_width = cs->getWidth();

            // caso a maquina seja MASTER
            if(comm_rank == MASTER){

                // designa que uma maquina execute um Flow::execute()
                char word_execute_send[23];
                int dest_ = 1;
                sprintf(word_execute_send, "%d|%d:%d|%lf", dest_, this->flows[i]->source.x, this->flows[i]->source.y, this->flows[i]->getFlowRate());
                // cout << "rank " << comm_rank << " word_execute_send " << word_execute_send << endl;

                for(int dest = 1; dest <= comm_workers; dest++){
                    MPI_Send(word_execute_send, 23, MPI_CHAR, dest, 3000, mpi_comm);
                }

                // de quem a MASTER devera' esperar a confirmacao
                int rank__;
                MPI_Recv(&rank__, 1, MPI_INT, dest_, 1000+i, mpi_comm, &mpi_status);

                char word_barrier;
                // recebe a confirmacao de final de execucao
                MPI_Recv(&word_barrier, 1, MPI_CHAR, dest_, 2000+i, mpi_comm, &mpi_status);
                MPI_Recv(&word_barrier, 1, MPI_CHAR, rank__, 4000+i, mpi_comm, &mpi_status);
            }
            // caso a maquina seja SLAYER
            else{
                char word_execute_recv[23];

                MPI_Recv(word_execute_recv, 23, MPI_CHAR, MASTER, 3000, mpi_comm, &mpi_status);
                t1.getFinish();

                // cout << "rank " << comm_rank << " word_execute_recv " << word_execute_recv << endl;
                char *rank_c = strtok(word_execute_recv, "|:");
                char *x_c = strtok(NULL, ":");
                char *y_c = strtok(NULL, "|");
                char *flow_rate_c = strtok(NULL, "|:");
                int rank_ = atoi(rank_c);
                int x_ = atoi(x_c);
                int y_ = atoi(y_c);
                int flow_rate_ = atoi(flow_rate_c);

                Time t2 = Time();
                // se a maquina for a que armazena a celula a ser fluxionada, ela executa o fluxo
                if(comm_rank == rank_){

                    this->flows[i]->setLastExecute(this->flows[i]->execute());

                    int count_neighbors_send, y_send;
                    T last_execute_send;

                    char word_target_send[23];

                    int rank__ = rank_;

                    count_neighbors_send = 0;
                    last_execute_send = 0;
                    sprintf(word_target_send, "%d|%d|%lf|%d", rank__, count_neighbors_send,
                        last_execute_send, y_send);

                    MPI_Send(&rank__, 1, MPI_INT, MASTER, 1000+i, MPI_COMM_WORLD);

                    // envia palavra "fake" com count_neighbors_send = 0, ou seja, nao havera execucao de fluxo fora daqui
                    for(int dest_ = 1; dest_ <= comm_workers; dest_++){
                        MPI_Send(&word_target_send, 23, MPI_CHAR, dest_, i, MPI_COMM_WORLD);
                    }

                    for(int j = 0; j < 15; j++){
                        this->flows[i]->setLastExecute(this->flows[i]->execute());
                    }

                    char word_barrier = 't';
                    MPI_Send(&word_barrier, 1, MPI_CHAR, MASTER, 2000+i, MPI_COMM_WORLD);
                }

                Time t3 = Time();
                t3.getInit();
                int rank__;
                int count_neighbors_recv, y_recv;
                T last_execute_recv;
                Attribute<T> attrib_tmp;

                char word_target_recv[23]; // word_target_recv = rank|count|last_execute|y

                MPI_Recv(word_target_recv, 23, MPI_CHAR, rank_, i, MPI_COMM_WORLD, &mpi_status);
                t3.getFinish();
                wait_time += t3.getTotalTime();

                // cout << i << " rank " << comm_rank << " word_execute_recv " << word_execute_recv << endl;
                rank_c = strtok(word_target_recv, "|");
                char *count_c = strtok(NULL, "|");
                char *last_execute_c = strtok(NULL, "|");
                y_c = strtok(NULL, "|");

                rank__ = atoi(rank_c);
                count_neighbors_recv = atoi(count_c);
                last_execute_recv = atof(last_execute_c);
                y_recv = atoi(y_c);

                // cout << " - " << comm_rank << endl;

                if(comm_rank == rank__){
                    t2.getInit();
                    // cout << "rank " << comm_rank << " last_execute_recv " << last_execute_recv << endl;
                    for(int j = 0; j < count_neighbors_recv; j++){
                        attrib_tmp = cs->memoria[y_recv + j].getAttribute();
                        attrib_tmp.setValue(attrib_tmp.getValue() + last_execute_recv);
                        cs->memoria[y_recv + j].setAttribute(attrib_tmp);
                    }

                    // envia confirmacao de final de execucao
                    char word_barrier = 't';
                    MPI_Send(&word_barrier, 1, MPI_CHAR, MASTER, 4000+i, MPI_COMM_WORLD);

                    t2.getFinish();
                    service_time += t2.getTotalTime();
                }

            }
        }
    }

    // calcula a media do tempo de servico e espera entre as maquinas
    if(comm_rank == MASTER){
        double service_wait_time_average[comm_workers][2];

        for(int source = 1; source <= comm_workers; source++){
            MPI_Recv(service_wait_time_average[source-1], 2, MPI_DOUBLE, source, 8000, mpi_comm, &mpi_status);
        }

        double service_time_average = 0;
        double wait_time_average = 0;

        for(int i = 0; i < comm_workers; i++){
            service_time_average += service_wait_time_average[i][0];
            wait_time_average += service_wait_time_average[i][1];
        }

        cout << "s_time " << service_time_average << endl;
        cout << "w_time " << wait_time_average << endl;

    } else {
        double ser_wai_t_avg[2] = {service_time, wait_time};

        MPI_Send(ser_wai_t_avg, 2, MPI_DOUBLE, MASTER, 8000, MPI_COMM_WORLD);
    }
}


template class Model<double>;
