//
// Created by Michał Zmyślony on 07/10/2021.
//

#ifndef VECTOR_SLICER_VALARRAYCONVERSION_H
#define VECTOR_SLICER_VALARRAYCONVERSION_H
#include <valarray>

std::valarray<double> itodArray(const std::valarray<int>& intArray);
std::valarray<int> dtoiArray(const std::valarray<double>& doubleArray);

#endif //VECTOR_SLICER_VALARRAYCONVERSION_H
