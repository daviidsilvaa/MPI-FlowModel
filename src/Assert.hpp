
#ifndef ASSERT_HPP
#define ASSERT_HPP

#include "mpi.h"
#include "CellularSpace.hpp"
#include "Abstraction.hpp"

template<class T>
void assert(const MPI_Comm &mpi_comm, CellularSpace<T> *cs, const double &assert_value, bool *bool_value){
	int comm_rank, comm_size;

	MPI_Comm_rank(mpi_comm, &comm_rank);
	MPI_Comm_size(mpi_comm, &comm_size);
	MPI_Status mpi_status;

	// assert() de conservacao de fluxo
    if(comm_rank == MASTER){
        // para cada maquina, MASTER requisita o somatorio resultante das execucoes

        T temp, acumulated_value_recv = 0;
        for(int source = 1; source < comm_size; source++){
            MPI_Recv(&temp, 1, ConvertType(getAbstractionDataType<T>()), source, source, MPI_COMM_WORLD, &mpi_status);
            acumulated_value_recv += temp;
            // cout << "acumulado " << source << ": " << acumulated_value_recv << "\t" << __LINE__ << endl;
        }

        // verifica se os valores simulados foram conservados
		if((acumulated_value_recv - assert_value) < 0.01){
			*bool_value = true;
        } else {
            *bool_value = false;
        }
        // assert((acumulated_value_recv - ASSERT_VALUE) < 0.001);

    } else {
        // calcula o somatorio dos valores atributos resultante na particao do espaco celular
        T acumulated_value_send = 0;

        for(int i = 0; i < cs->getHeight()*cs->getWidth(); i++){
            acumulated_value_send += cs->memoria[i].attribute.value;
        }

        // envia o valor somado para o MASTER, para que o assert() seja feito
        MPI_Send(&acumulated_value_send, 1, ConvertType(getAbstractionDataType<T>()), MASTER, comm_rank, MPI_COMM_WORLD);
    }
}

#endif
