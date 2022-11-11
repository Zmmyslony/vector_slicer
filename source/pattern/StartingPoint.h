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
    int previouslyFoundPoint = 0;

    void updateListOfPoints(FilledPattern &pattern);

    void lookForAPoint(FilledPattern &pattern);

    void trySearchingForAPoint(FilledPattern &pattern);

    void findStartPointTotallyRandomly(FilledPattern &pattern);

    void findStartPointConsecutively(FilledPattern &pattern);

    void findStartPointSemiRandomly(FilledPattern &pattern);

public:
    explicit StartingPoint();

    void refresh();

    std::valarray<int> positions;

    std::valarray<int> findStartPoint(FilledPattern &pattern);
};


#endif //VECTOR_SLICER_STARTINGPOINT_H
