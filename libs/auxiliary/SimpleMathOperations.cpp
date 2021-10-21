//
// Created by Michał Zmyślony on 23/09/2021.
//

#include "SimpleMathOperations.h"
#include <cmath>

int roundUp(double number) {
    if (number > 0) {
        return (int)ceil(number);
    }
    else {
        return (int)floor(number);
    }
}

int sgn(double number) {
    return (number > 0) - (number < 0);
}

double decimalPart(double number) {
    return number - floor(number);
}