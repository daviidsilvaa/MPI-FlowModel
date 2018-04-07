
#ifndef MPIIMPL_HPP
#define MPIIMPL_HPP

#include "mpi.h"
#include "Abstraction.hpp"
#include <vector>

static MPI_Datatype ConvertType(Abstraction::DataType type){
	switch (type){
	case Abstraction::type_char: return MPI_CHAR;
	case Abstraction::type_unsigned_char: return MPI_UNSIGNED_CHAR;
	case Abstraction::type_short: return MPI_SHORT;
	case Abstraction::type_unsigned_short: return MPI_UNSIGNED_SHORT;
	case Abstraction::type_int: return MPI_INT;
	case Abstraction::type_unsigned_int: return MPI_UNSIGNED;
	case Abstraction::type_long: return MPI_LONG;
	case Abstraction::type_unsigned_long: return MPI_UNSIGNED_LONG;
	case Abstraction::type_float: return MPI_FLOAT;
	case Abstraction::type_double: return MPI_DOUBLE;
	};
	throw std::runtime_error("MPI_Datatype Convert(Abstraction::DataType) failed");
}

void SendImpl(void *data, int count, Abstraction::DataType type, int dest, int tag);
void ReceiveImpl(void *data, int count, Abstraction::DataType type, int src, int tag);

template<class T>
void Send(std::vector<T> &vec, int count, int dest, int tag = 0){
	SendImpl(vec.data(), count, getAbstractionDataType<T>(), dest, tag);
}

template<class T>
void Receive(std::vector<T> &vec, int count, int src, int tag = 0){
	ReceiveImpl(vec.data(), count, getAbstractionDataType<T>(), src, tag);
};

#endif
