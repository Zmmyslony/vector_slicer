////
//// Created by Michał Zmyślony on 21/10/2021.
////
//
//#ifndef VECTOR_SLICER_PATHFILLINGORDER_H
//#define VECTOR_SLICER_PATHFILLINGORDER_H
//
//#include <vector>
//#include <valarray>
//#include "Path.h"
//#include "FilledPattern.h"
//#include <string>
//#include "../auxiliary/GCodeFile.h"
//
//class PathFillingOrder {
//    std::vector<Path> sequenceOfPaths;
//    GCodeFile gCodeFile;
//    std::valarray<int> dimensions;
//
//    std::vector<std::valarray<int>> startAndEndPositions;
//    std::vector<int> orderingOfPaths;
//    std::vector<bool> isPathInCorrectDirection;
//
//    double getMovedDistance();
//
//    void startGeneratingPrintPathsFrom(std::valarray<int> startingPoint);
//
//public:
//    explicit PathFillingOrder(FilledPattern &pattern);
//
//    void generateBestFillingOrderAndExportToPath(const std::string &path);
//
//    std::valarray<int> findBestStartingPoints();
//};
//
//
//#endif //VECTOR_SLICER_PATHFILLINGORDER_H
