
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

	int GetKey(void);
	void SetKey(const int &key);

	T GetValue(void);
	void SetValue(const T &value);
};

template<class T>
MPI_Datatype Make_MPI_Atribute();

#endif
