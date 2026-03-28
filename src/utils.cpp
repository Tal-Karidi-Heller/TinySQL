#include "utils.h"
#include "tokenizer.h"


template<typename T>
bool VectorIterator<T>::not_empty() {
    return start != end;
}

template<typename T>
T VectorIterator<T>::next() {
    if (!this->not_empty())
        throw std::runtime_error("Attempted to get_next on exhausted iterator");
    return *(start++);
}

template<typename T>
T VectorIterator<T>::peek() {
    if (!this->not_empty())
        throw std::runtime_error("Attempted to peek on exhausted iterator");
    return *start;
}

template class VectorIterator<Token>;
