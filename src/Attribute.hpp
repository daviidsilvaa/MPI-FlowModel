
#ifndef ATTRIBUTE_HPP
#define ATTRIBUTE_HPP

#include "mpi.h"
#include "MPIImpl.hpp"

template<class T>
class Attribute{
public:
    int key;
    T value;

    Attribute();

    Attribute(const int &key, const T &value);

    Attribute(const Attribute<T> &attrib);

    ~Attribute();

    Attribute<T>& operator=(const Attribute<T> &attribute);

	int getKey(void);
	void setKey(const int &key);

	T getValue(void);
	void setValue(const T &value);
};

template<class T>
MPI_Datatype Make_MPI_Atribute();

#endif
