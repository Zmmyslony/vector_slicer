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
// Created by Michał Zmyślony on 29/09/2021.
//

#ifndef VECTOR_SLICER_QUANTIFYPATTERN_H
#define VECTOR_SLICER_QUANTIFYPATTERN_H

#include "FilledPattern.h"

class QuantifyPattern {
    FilledPattern pattern;
    double emptySpots = 0;
    double averageOverlap = 0;
    double directorDisagreement = 0;
    double numberOfPaths = 0;

    double calculateEmptySpots();

    double calculateAverageOverlap();

    double calculateDirectorDisagreement();

    double calculateNumberOfPaths();

public:
    explicit QuantifyPattern(FilledPattern pattern);

    double calculateCorrectness(double emptySpotWeight, double overlapWeight, double directorWeight, double pathWeight,
                                double emptySpotExponent, double overLapExponent, double directorExponent,
                                double pathExponent) const;

    double
    calculateCorrectness(double emptySpotWeight, double overlapWeight, double directorWeight, double pathWeight) const;


};


#endif //VECTOR_SLICER_QUANTIFYPATTERN_H
