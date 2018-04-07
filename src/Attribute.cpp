
#include "Attribute.hpp"
#include "MPIImpl.hpp"

template<class T>
Attribute<T>::Attribute(){ }

template<class T>
Attribute<T>::Attribute(const int &key, const T &value){
    this->SetKey(key);
    this->SetValue(value);
}

template<class T>
Attribute<T>::Attribute(const Attribute<T> &attrib){
    this->SetKey(attrib.key);
    this->SetValue(attrib.value);
}

template<class T>
Attribute<T>::~Attribute(){ }

template<class T>
Attribute<T>& Attribute<T>::operator=(const Attribute<T> &attribute){
    if(this != &attribute){
        this->SetKey(attribute.key);
        this->SetValue(attribute.value);
    }
    return *this;
}

template<class T>
int Attribute<T>::GetKey(void){
    return this->key;
}
template<class T>
void Attribute<T>::SetKey(const int &key){
    this->key = key;
}

template<class T>
T Attribute<T>::GetValue(void){
    return this->value;
}
template<class T>
void Attribute<T>::SetValue(const T &value){
    this->value = value;
}

// "enganando o compilador"
template class Attribute<double>;
template class Attribute<int>;

template<class T>
MPI_Datatype Make_MPI_Atribute(){
	MPI_Datatype MPI_Attribute;

	const int nitens = 2;
	MPI_Datatype blocktypes[2] = {MPI_INT, ConvertType(getAbstractionDataType<T>())};
	int blocklengths[2] = {1, 1};
	MPI_Aint offsets[2] = {offsetof(Attribute<T>, key), offsetof(Attribute<T>, value)};

	MPI_Type_create_struct(nitens, blocklengths, offsets, blocktypes, &MPI_Attribute);
	MPI_Type_commit(&MPI_Attribute);

	return MPI_Attribute;
}
