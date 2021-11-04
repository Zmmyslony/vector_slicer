//
// Created by Michał Zmyślony on 15/10/2021.
//

#include "ValarrayOperations.h"
#include "ValarrayConversion.h"

double generalNorm(const std::valarray<double>& array, const double& exponent) {
    double sum = 0;
    for (auto& element : array) {
        sum += pow(element, exponent);
    }
    return pow(sum, 1 / exponent);
}

double norm(const std::valarray<double>& array) {
    return generalNorm(array, 2);
}

double norm(const std::valarray<int>& array) {
    return norm(itodArray(array));
}

std::valarray<double> generalNormalize(const std::valarray<double>& array, const double& exponent) {
    return array / generalNorm(array, exponent);
}

std::valarray<double> normalize(const std::valarray<double>& array) {
    return generalNormalize(array, 2);
}

std::valarray<double> normalize(const std::valarray<int>& array) {
    return normalize(itodArray(array));
}