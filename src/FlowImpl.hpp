
#ifndef FLOWIMPL_HPP
#define FLOWIMPL_HPP

#include "Flow.hpp"

template<class T>
class FlowImpl : public Flow<T>{
public:
    Cell<T> source;
    int target[NEIGHBORS+NEIGHBORS];
    int count_targets;
    double flow_rate;
    double last_execute;

    FlowImpl();
    FlowImpl(const Cell<T> &cell, const double &flow_rate);
    FlowImpl(const FlowImpl<T> &flow);

    ~FlowImpl();

    FlowImpl<T>& operator=(const FlowImpl<T> &flow);
};

#endif
