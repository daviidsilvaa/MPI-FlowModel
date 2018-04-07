
#include "MPIImpl.hpp"
#include <stdexcept>

void SendImpl(void *data, int count, Abstraction::DataType type, int dest, int tag){
	if (MPI_Send(data, count, ConvertType(type), dest, tag, MPI_COMM_WORLD) != MPI_SUCCESS)
		throw std::runtime_error("MPI_Send failed");
}

void ReceiveImpl(void *data, int count, Abstraction::DataType type, int src, int tag){
	MPI_Status status;
	if (MPI_Recv(data, count, ConvertType(type), src, tag, MPI_COMM_WORLD, &status) != MPI_SUCCESS)
		throw std::runtime_error("MPI_Recv failed");
}
