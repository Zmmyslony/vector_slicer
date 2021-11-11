//
// Created by Michał Zmyślony on 27/09/2021.
//

#ifndef VECTOR_SLICER_FILLINGPATTERNS_H
#define VECTOR_SLICER_FILLINGPATTERNS_H

#include "FilledPattern.h"
#include "StartingPoint.h"

bool tryGeneratingNewPath(FilledPattern &pattern, StartingPoint &startingPoint);

void fillWithPaths(FilledPattern &pattern);

#endif //VECTOR_SLICER_FILLINGPATTERNS_H
