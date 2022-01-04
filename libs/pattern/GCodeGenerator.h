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
#include "../auxiliary/GCodeFile.h"

class GCodeGenerator {
    std::vector<Path> sequenceOfPaths;
    GCodeFile gCodeFile;

    std::vector<std::valarray<int>> startAndEndPositions;
    std::vector<int> orderingOfPaths;
    std::vector<bool> isPathInCorrectDirection;

    double getMovedDistance();

    void startGeneratingPrintPathsFrom(std::valarray<int> startingPoint);

public:
    explicit GCodeGenerator(FilledPattern pattern);

    void exportToPath(const std::string &filename);

    std::valarray<int> findBestStartingPoints(const std::valarray<int> &dimensions);
};


#endif //VECTOR_SLICER_GCODEGENERATOR_H
