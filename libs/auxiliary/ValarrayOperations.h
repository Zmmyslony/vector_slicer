//
// Created by Michał Zmyślony on 15/10/2021.
//

#ifndef VECTOR_SLICER_VALARRAYOPERATIONS_H
#define VECTOR_SLICER_VALARRAYOPERATIONS_H

#include <valarray>

double generalNorm(const std::valarray<double> &array, const double &exponent);

double norm(const std::valarray<double> &array);

double norm(const std::valarray<int> &array);

std::valarray<double> generalNormalize(const std::valarray<double> &array, const double &exponent);

std::valarray<double> normalize(const std::valarray<double> &array);

std::valarray<double> normalize(const std::valarray<int> &array);

#endif //VECTOR_SLICER_VALARRAYOPERATIONS_H
