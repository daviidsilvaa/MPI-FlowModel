
#include "FlowImpl.hpp"

template<class T>
FlowImpl<T>::FlowImpl(){ }

template<class T>
FlowImpl<T>::FlowImpl(const Cell<T> &cell, const double &flow_rate){
    this->source = cell;
    this->count_targets = cell.count_neighbors;
    this->flow_rate = flow_rate;

    // setando os indices dos targets
    for(int i = 0; i < this->count_targets; i++){
        // x do target
        target[i] = cell.neighbors[i];
        // y do target
        target[NEIGHBORS + i] = cell.neighbors[NEIGHBORS + i];
    }
}

template<class T>
FlowImpl<T>::FlowImpl(const FlowImpl<T> &flow){
    this->source = flow.source;
    this->count_targets = flow.count_targets;
    for(int i = 0; i < this->count_targets; i++){
        target[i] = flow.target[i];
        target[NEIGHBORS + i] = flow.target[NEIGHBORS + i];
    }
    this->flow_rate = flow.flow_rate;
    this->last_execute = flow.last_execute;
}

template<class T>
FlowImpl<T>::~FlowImpl(){ }

template<class T>
FlowImpl<T>& FlowImpl<T>::operator=(const FlowImpl<T> &flow){
    if(this != &flow){
        this->source = flow.source;
        this->count_targets = flow.count_targets;
        for(int i = 0; i < this->count_targets; i++){
            target[i] = flow.target[i];
            target[NEIGHBORS + i] = flow.target[NEIGHBORS + i];
        }
        this->flow_rate = flow.flow_rate;
        this->last_execute = flow.last_execute;
    }
    return *this;
}

template<class T>
void FlowImpl<T>::setSource(const Cell<T> &source){
    this->source = source;
}
template<class T>
Cell<T> FlowImpl<T>::getSource(){
    return this->source;
}
template<class T>
void FlowImpl<T>::setCountTargets(const int &count_targets){
    this->count_targets = count_targets;
}
template<class T>
int FlowImpl<T>::getCountTargets(){
    return this->count_targets;
}
template<class T>
void FlowImpl<T>::setFlowRate(const double &flow_rate){
    this->flow_rate = flow_rate;
}
template<class T>
double FlowImpl<T>::getFlowRate(){
    return this->flow_rate;
}
template<class T>
void FlowImpl<T>::setLastExecute(const double &last_execute){
    this->last_execute = last_execute;
}
template<class T>
double FlowImpl<T>::getLastExecute(){
    return this->last_execute;
}

template class FlowImpl<double>;
template class FlowImpl<int>;
