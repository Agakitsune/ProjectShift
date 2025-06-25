
#ifndef ALCHEMIST_MATH_MATRIX_EXT_H
#define ALCHEMIST_MATH_MATRIX_EXT_H

#include "math/matrix/mat4.hpp"
#include "math/vector/ext.hpp"

#include <iostream>

std::ostream &operator<<(std::ostream &os, const mat4 &m) {
    os << "mat4(";
    for (int i = 0; i < 4; ++i) {
        os << m.columns[i];
        if (i < 3) {
            os << ", ";
        }
    }
    os << ")";
    return os;
}

#endif // ALCHEMIST_MATH_MATRIX_EXT_H
