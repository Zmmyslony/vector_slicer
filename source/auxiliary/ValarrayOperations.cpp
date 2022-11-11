// 2022, Michał Zmyślony, mlz22@cam.ac.uk.
//
// Please cite Michał Zmyślony and Dr John Biggins if you use any part of this code in work you publish or distribute.
//
// This file is part of Vector Slicer.
//
// Vector Slicer is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//
// Vector Slicer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with Foobar. If not, see <https://www.gnu.org/licenses/>.

//
// Created by Michał Zmyślony on 15/10/2021.
//

#include "ValarrayOperations.h"
#include <stdexcept>
#include <iostream>

std::valarray<double> itodArray(const std::valarray<int> &int_array) {
    std::valarray<double> double_array(int_array.size());
    for (int i = 0; i < int_array.size(); i++) {
        double_array[i] = (double) int_array[i];
    }
    return double_array;
}

std::valarray<int> dtoiArray(const std::valarray<double> &double_array) {
    std::valarray<int> int_array(double_array.size());
    for (int i = 0; i < double_array.size(); i++) {
        int_array[i] = lround(double_array[i]);
    }
    return int_array;
}

double generalNorm(const std::valarray<double> &array, const double &exponent) {
    double sum = 0;
    for (auto &element: array) {
        sum += pow(element, exponent);
    }
    return pow(sum, 1 / exponent);
}

double norm(const std::valarray<double> &array) {
    return generalNorm(array, 2);
}

double norm(const std::valarray<int> &array) {
    return norm(itodArray(array));
}

std::valarray<double> generalNormalize(const std::valarray<double> &array, const double &exponent) {
    return array / generalNorm(array, exponent);
}

std::valarray<double> normalize(const std::valarray<double> &array) {
    return generalNormalize(array, 2);
}

std::valarray<double> normalize(const std::valarray<int> &array) {
    return normalize(itodArray(array));
}

double dot(const std::valarray<double> &array_first, const std::valarray<double> &array_second) {
    double dot_product = 0;
    if (array_first.size() != array_second.size()) {
        throw std::invalid_argument("Dot: Dotted array are of different size.\n");
    }
    for (int i = 0; i < array_first.size(); i++) {
        dot_product += array_first[i] * array_second[i];
    }
    return dot_product;
}

double dot(const std::valarray<int> &array_first, const std::valarray<int> &array_second) {
    return dot(itodArray(array_first), itodArray(array_second));
}

std::valarray<double> perpendicular(std::valarray<double> vector) {
    if (vector.size() != 2) {
        throw std::invalid_argument("Perpendicular: Size of the valarray not equal to 2.\n");
    }
    return std::valarray<double>({-vector[1], vector[0]});
}


std::valarray<int> perpendicular(const std::valarray<int> &vector) {
    return dtoiArray(perpendicular(itodArray(vector)));
}


void printArray(const std::vector<std::valarray<int>> &array) {
    for (auto &row: array) {
        std::cout << "(";
        for (auto &element: row) {
            std::cout << element << ", ";
        }
        std::cout << "), ";
    }
    std::cout << std::endl;
}

void printArray(const std::vector<std::valarray<double>> &array) {
    for (auto &row: array) {
        std::cout << "(";
        for (auto &element: row) {
            std::cout << element << ", ";
        }
        std::cout << "), ";
    }
    std::cout << std::endl;
}

std::valarray<int> uinttointArray(const std::valarray<unsigned int> &uint_array) {
    std::valarray<int> int_array(uint_array.size());
    for (int i = 0; i < uint_array.size(); i++) {
        int_array[i] = lround(uint_array[i]);
    }
    return int_array;
}