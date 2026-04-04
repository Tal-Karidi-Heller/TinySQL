//
// Created by Tal Karidi on 27/03/2026.
//

#ifndef MY_PROJECT_UTILS_H
#define MY_PROJECT_UTILS_H
#include <vector>
#include <ostream>
#include <iostream>
#include <optional>   // Added
#include <stdexcept>   // Added
#include <exception>  // Added
#include "token.h"
#include "table.h"

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

    inline bool not_empty() {
        return start != end;
    };

    inline bool empty() {
        return !not_empty();
    }

    inline T next() {
        if (!this->not_empty())
            throw std::runtime_error("Attempted to get_next on exhausted iterator");
        return *(start++);
    };

    inline T peek() {
        if (!this->not_empty())
            throw std::runtime_error("Attempted to peek on exhausted iterator");
        return *start;
    };

    inline void print_vector() {
        int i = 0;
        auto c = start;
        while (c != end) {
            std::cout << *c;
            ++c;
        }
    };

    inline bool expect(const T &value, const bool take_out) {
        if (this->empty() || this->peek() != value)
            return false;
        if (take_out)
            this->next();
        return true;
    }

    inline bool expect(const T &value) {
        return this->expect(value, true);
    }
};

template class VectorIterator<std::string>;
template class VectorIterator<Token>;

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const std::optional<T>& v) {
    if (!v.has_value())
        os << "NULL";
    else
        os << v.value();

    return os;
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
    os << "[ ";
    for (typename std::vector<T>::size_type i = 0; i < v.size(); ++i) {
        os << v[i];
        if (i != v.size() - 1) {
            os << ", ";
        }
    }
    os << " ]";
    return os;
}

template <typename  T>
inline bool operator==(const std::vector<T>& _1, const std::vector<T> &_2) {
    if (_1.size() != _2.size())
        return false;

    for (int i = 0; i < _1.size(); i++) {
        if (_1[i] != _2[i])
            return false;
    }

    return true;
}

inline std::optional<int> convert_to_number(const std::string &string) {
    try {
        int num = std::stoi(string);
        return num;
    } catch (const std::invalid_argument &e) {
        return std::nullopt;
    } catch (const std::out_of_range &e) {
        return std::nullopt;
    }
}

class ExpectedException : public std::exception {
public:
    virtual ~ExpectedException() override = default;
    virtual const char* what() const noexcept override = 0;
};


#endif
