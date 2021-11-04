//
// Created by Michał Zmyślony on 21/10/2021.
//

#ifndef VECTOR_SLICER_GCODEGENERATOR_H
#define VECTOR_SLICER_GCODEGENERATOR_H

#include <vector>
#include <valarray>
#include "Path.h"
#include "FilledPattern.h"
#include <string>

class GcodeGenerator {
    std::vector<Path> sequenceOfPaths;
    std::string header;
    std::string footer;
    std::string printPathsGcode;

    std::vector<std::valarray<int>> startAndEndPositions;
    std::vector<int> orderingOfPaths;
    std::vector<bool> isPathInCorrectDirection;

    double getMovedDistance();
    void startGeneratingPrintPathsFrom(std::valarray<int> startingPoint);
public:
    explicit GcodeGenerator(FilledPattern pattern);
    void exportToPath(std::string gcodePath);

    std::valarray<int> findBestStartingPoints();
};


#endif //VECTOR_SLICER_GCODEGENERATOR_H
