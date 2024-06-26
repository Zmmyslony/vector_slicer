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

vald itod(const vali &int_array) {
    vald double_array(int_array.size());
    for (int i = 0; i < int_array.size(); i++) {
        double_array[i] = (double) int_array[i];
    }
    return double_array;
}

vali dtoi(const vald &double_array) {
    vali int_array(double_array.size());
    for (int i = 0; i < double_array.size(); i++) {
        int_array[i] = lround(double_array[i]);
    }
    return int_array;
}

double generalNorm(const vald &array, const double &exponent) {
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

double norm(const vald &array) {
    return generalNorm(array, 2);
}

double norm(const vali &array) {
    return norm(itod(array));
}

vald generalNormalize(const vald &array, const double &exponent) {
    return array / generalNorm(array, exponent);
}

vald normalize(const vald &array) {
    return generalNormalize(array, 2);
}

vald normalize(const vali &array) {
    return normalize(itod(array));
}

double dot(const vald &array_first, const vald &array_second) {
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

double angle(const vald &array_first, const vald &array_second) {
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


vald perpendicular(const vald &vector) {
    if (vector.size() != 2) {
        throw std::invalid_argument("Perpendicular: Size of the valarray not equal to 2.\n");
    }
    return {-vector[1], vector[0]};
}

vali perpendicular(const vali &vector) {
    if (vector.size() != 2) {
        throw std::invalid_argument("Perpendicular: Size of the valarray not equal to 2.\n");
    }
    return {-vector[1], vector[0]};
}

double cross(const vald &vector_first, const vald &vector_second) {

    return vector_first[0] * vector_second[1] - vector_first[1] * vector_second[0];
}


void printArray(const std::vector<vali> &array) {
    for (auto &row: array) {
        std::cout << "(";
        for (auto &element: row) {
            std::cout << element << ", ";
        }
        std::cout << "), ";
    }
    std::cout << std::endl;
}

void printArray(const std::vector<vald> &array) {
    for (auto &row: array) {
        std::cout << "(";
        for (auto &element: row) {
            std::cout << element << ", ";
        }
        std::cout << "), ";
    }
    std::cout << std::endl;
}

vald multiply(matrix_d mat, vald vec) {
    vald result(mat.size());
    for (int i = 0; i < mat.size(); i++) {
        if (mat[i].size() != vec.size()) {
            throw std::runtime_error("Uneven sizes in matrix and vector multiplication.");
        }
        double element = 0;
        for (int j = 0; j < mat[i].size(); j++) {
            element += mat[i][j] * vec[j];
        }
        result[i] = element;
    }
    return result;
}

matrix_d tensor(vald vec_first, vald vec_second) {
    matrix_d result(vald(vec_second.size()), vec_first.size());
    for (int i = 0; i < vec_first.size(); i++) {
        for (int j = 0; j < vec_second.size(); j++) {
            result[i][j] = vec_first[i] * vec_second[j];
        }
    }
    return result;
}