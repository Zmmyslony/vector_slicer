// Copyright (c) 2021-2025, Michał Zmyślony, mlz22@cam.ac.uk.
//
// Please cite following publication if you use any part of this code in work you publish or distribute:
// [1] Michał Zmyślony M., Klaudia Dradrach, John S. Biggins,
//    Slicing vector fields into tool paths for additive manufacturing of nematic elastomers,
//    Additive Manufacturing, Volume 97, 2025, 104604, ISSN 2214-8604, https://doi.org/10.1016/j.addma.2024.104604.
//
// This file is part of Vector Slicer.
//
// Vector Slicer is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
// later version.
//
// Vector Slicer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License along with Vector Slicer.
// If not, see <https://www.gnu.org/licenses/>.

//
// Created by Michał Zmyślony on 15/10/2021.
//

#define _USE_MATH_DEFINES

#include "valarray_operations.h"

#include <stdexcept>
#include <math.h>
#include <iostream>
#include <string>

vecd itod(const veci &int_array) {
    vecd double_array(int_array.size());
    for (int i = 0; i < int_array.size(); i++) {
        double_array[i] = (double) int_array[i];
    }
    return double_array;
}

veci dtoi(const vecd &double_array) {
    veci int_array(double_array.size());
    for (int i = 0; i < double_array.size(); i++) {
        int_array[i] = lround(double_array[i]);
    }
    return int_array;
}

double generalNorm(const vecd &array, const double &exponent) {
    double sum = 0;
    for (auto &element: array) {
        sum += pow(element, exponent);
    }
    if (sum == 0) {
        return 0;
    } else {
        return pow(sum, 1 / exponent);
    }
}

double norm(const vecd &array) {
    return generalNorm(array, 2);
}

double norm(const veci &array) {
    return norm(itod(array));
}


coord_d matrix_multiply(matrix_d mat, coord_d vec) {
    coord_d result;
    result.x = vec.x * mat[0][0] + vec.y * mat[1][0];
    result.y = vec.x * mat[0][1] + vec.y * mat[1][1];

    return result;
}

matrix_d tensor(coord_d vec_first, coord_d vec_second) {
    matrix_d result(2, vecd(2));
    result[0][0] = vec_first.x * vec_second.x;
    result[1][0] = vec_first.y * vec_second.x;
    result[0][1] = vec_first.x * vec_second.y;
    result[1][1] = vec_first.y * vec_second.y;

    return result;
}