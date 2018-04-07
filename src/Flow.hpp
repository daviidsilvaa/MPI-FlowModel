
#ifndef FLOW_HPP
#define FLOW_HPP

#include "Cell.hpp"

template<class T>
class Flow{
public:
    virtual double execute() = 0;

    ~Flow();
};

#endif
