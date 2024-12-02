// Copyright (c) 2021-2023, Michał Zmyślony, mlz22@cam.ac.uk.
//
// Please cite Michał Zmyślony and Dr John Biggins if you use any part of this code in work you publish or distribute.
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

vecd generalNormalize(const vecd &array, const double &exponent) {
    double norm = generalNorm(array, exponent);
    vecd normalized_array;
    normalized_array.reserve(array.size());
    for (auto & val: array) {
        normalized_array.emplace_back(val / norm);
    }
    return normalized_array ;
}

vecd normalize(const vecd &array) {
    return generalNormalize(array, 2);
}

vecd normalize(const veci &array) {
    return normalize(itod(array));
}

double dot(const vecd &array_first, const vecd &array_second) {
    double dot_product = 0;
    if (array_first.size() != array_second.size()) {
        throw std::invalid_argument(
                "Dot: Dotted array are of different sizes" + std::to_string(array_first.size()) + " and "
                + std::to_string(array_second.size()) + "\n");
    }
    for (int i = 0; i < array_first.size(); i++) {
        dot_product += array_first[i] * array_second[i];
    }
    return dot_product;
}

double angle(const coord_d &array_first, const coord_d &array_second) {
    double dot_product = dot(array_first, array_second);
    double length_first = norm(array_first);
    double length_second = norm(array_second);
    if (length_first == 0 || length_second == 0) {
        return M_PI;
    }

    double cosine = dot_product / (length_first * length_second);
    if (cosine > 1) {
        return 0;
    }


    return acos(cosine);
}


vecd perpendicular(const vecd &vector) {
    if (vector.size() != 2) {
        throw std::invalid_argument("Perpendicular: Size of the valarray not equal to 2.\n");
    }
    return {-vector[1], vector[0]};
}

veci perpendicular(const veci &vector) {
    if (vector.size() != 2) {
        throw std::invalid_argument("Perpendicular: Size of the valarray not equal to 2.\n");
    }
    return {-vector[1], vector[0]};
}

double cross(const vecd &vector_first, const vecd &vector_second) {
    return vector_first[0] * vector_second[1] - vector_first[1] * vector_second[0];
}


void printArray(const std::vector<veci> &array) {
    for (auto &row: array) {
        std::cout << "(";
        for (auto &element: row) {
            std::cout << element << ", ";
        }
        std::cout << "), ";
    }
    std::cout << std::endl;
}

void printArray(const std::vector<vecd> &array) {
    for (auto &row: array) {
        std::cout << "(";
        for (auto &element: row) {
            std::cout << element << ", ";
        }
        std::cout << "), ";
    }
    std::cout << std::endl;
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