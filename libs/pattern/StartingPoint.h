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

public:
    explicit StartingPoint();

    std::valarray<int> positions;

    void refresh();

    void findStartPointConsecutively(FilledPattern &pattern);

    void findStartPointRandomly(FilledPattern &pattern);

    std::valarray<int> findStartPoint(FilledPattern &pattern);
};


#endif //VECTOR_SLICER_STARTINGPOINT_H
