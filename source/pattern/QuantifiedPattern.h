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

#ifndef VECTOR_SLICER_QUANTIFIEDPATTERN_H
#define VECTOR_SLICER_QUANTIFIEDPATTERN_H

#include "FilledPattern.h"

class QuantifiedPattern: FilledPattern {
    double empty_spots = 0;
    double average_overlap = 0;
    double director_disagreement = 0;
    double number_of_paths = 0;

    double calculateEmptySpots();

    double calculateAverageOverlap();

    double calculateDirectorDisagreement();

    double calculateNumberOfPaths();

public:
    explicit QuantifiedPattern(FilledPattern pattern);

    double disagreement(double empty_spot_weight, double overlap_weight, double director_weight, double path_weight,
                        double empty_spot_exponent, double over_lap_exponent, double director_exponent,
                        double path_exponent) const;

    double
    disagreement(double empty_spot_weight, double overlap_weight, double director_weight, double path_weight) const;


};


#endif //VECTOR_SLICER_QUANTIFIEDPATTERN_H
