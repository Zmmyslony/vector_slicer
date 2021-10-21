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
                                double emptySpotExponent, double overLapExponent, double directorExponent, double pathExponent) const;
    double calculateCorrectness(double emptySpotWeight, double overlapWeight, double directorWeight, double pathWeight) const;

    void printResults() const;

};


#endif //VECTOR_SLICER_QUANTIFYPATTERN_H
