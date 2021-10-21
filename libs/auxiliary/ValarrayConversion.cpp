//
// Created by Michał Zmyślony on 07/10/2021.
//

#include "ValarrayConversion.h"
#include <cmath>

std::valarray<double> itodArray(const std::valarray<int>& intArray) {
    std::valarray<double> doubleArray(intArray.size());
    for (int i = 0; i < intArray.size(); i++) {
        doubleArray[i] = (double)intArray[i];
    }
    return doubleArray;
}

std::valarray<int> dtoiArray(const std::valarray<double>& doubleArray) {
    std::valarray<int> intArray(doubleArray.size());
    for (int i = 0; i < doubleArray.size(); i++) {
        intArray[i] = lround(doubleArray[i]);
    }
    return intArray;
}