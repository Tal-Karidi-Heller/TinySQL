//
// Created by Tal Karidi on 27/03/2026.
//

#ifndef MY_PROJECT_UTILS_H
#define MY_PROJECT_UTILS_H
#include <vector>

template<typename T>
class VectorIterator {
private:
    typename std::vector<T>::iterator start;
    typename std::vector<T>::iterator end;

public:
    VectorIterator(typename std::vector<T>::iterator start,
                   typename std::vector<T>::iterator end) : start(start), end(end) {
    }

    VectorIterator(std::vector<T> vector) : VectorIterator(vector.begin(), vector.end()) {
    }

    bool not_empty();

    T next();

    T peek();
};

#endif
