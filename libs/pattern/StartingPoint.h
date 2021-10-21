//
// Created by Michał Zmyślony on 23/09/2021.
//

#ifndef VECTOR_SLICER_STARTINGPOINT_H
#define VECTOR_SLICER_STARTINGPOINT_H

#include "FilledPattern.h"
#include <valarray>


class StartingPoint {
    bool isStartingPointFound = false;
    bool areThereFillablePointsRemaining = true;
    int tries;

public:
    explicit StartingPoint();

    std::valarray<int> positions;
//    void findSuitableStartPointRandomly(FilledPattern& pattern);
    void findSuitableStartPointSemiRandomly(FilledPattern& pattern);

    std::valarray<int> findSuitableStartPoint(FilledPattern& pattern);
};


#endif //VECTOR_SLICER_STARTINGPOINT_H
