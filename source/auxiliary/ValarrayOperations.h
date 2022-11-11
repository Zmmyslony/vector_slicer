//
// Created by Michał Zmyślony on 15/10/2021.
//

#ifndef VECTOR_SLICER_VALARRAYOPERATIONS_H
#define VECTOR_SLICER_VALARRAYOPERATIONS_H

#include <valarray>
#include <vector>

std::valarray<double> itodArray(const std::valarray<int> &intArray);

std::valarray<int> dtoiArray(const std::valarray<double> &doubleArray);

double generalNorm(const std::valarray<double> &array, const double &exponent);

double norm(const std::valarray<double> &array);

double norm(const std::valarray<int> &array);

std::valarray<double> generalNormalize(const std::valarray<double> &array, const double &exponent);

std::valarray<double> normalize(const std::valarray<double> &array);

std::valarray<double> normalize(const std::valarray<int> &array);

double dot(const std::valarray<double> &array1, const std::valarray<double> &array2);

double dot(const std::valarray<int> &array1, const std::valarray<int> &array2);

std::valarray<double> perpendicular(std::valarray<double> vector);

std::valarray<int> perpendicular(const std::valarray<int> &vector);

void printArray(const std::vector<std::valarray<int>> &array);

void printArray(const std::vector<std::valarray<double>> &array);

std::valarray<int> uinttointArray(const std::valarray<unsigned int> &uintArray);

#endif //VECTOR_SLICER_VALARRAYOPERATIONS_H
