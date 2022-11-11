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

std::valarray<double> itodArray(const std::valarray<int> &intArray) {
    std::valarray<double> doubleArray(intArray.size());
    for (int i = 0; i < intArray.size(); i++) {
        doubleArray[i] = (double) intArray[i];
    }
    return doubleArray;
}

std::valarray<int> dtoiArray(const std::valarray<double> &doubleArray) {
    std::valarray<int> intArray(doubleArray.size());
    for (int i = 0; i < doubleArray.size(); i++) {
        intArray[i] = lround(doubleArray[i]);
    }
    return intArray;
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

double dot(const std::valarray<double> &array1, const std::valarray<double> &array2) {
    double dotProduct = 0;
    if (array1.size() != array2.size()) {
        throw std::invalid_argument("Dot: Dotted array are of different size.\n");
    }
    for (int i = 0; i < array1.size(); i++) {
        dotProduct += array1[i] * array2[i];
    }
    return dotProduct;
}

double dot(const std::valarray<int> &array1, const std::valarray<int> &array2) {
    return dot(itodArray(array1), itodArray(array2));
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

std::valarray<int> uinttointArray(const std::valarray<unsigned int> &uintArray) {
    std::valarray<int> intArray(uintArray.size());
    for (int i = 0; i < uintArray.size(); i++) {
        intArray[i] = lround(uintArray[i]);
    }
    return intArray;
}