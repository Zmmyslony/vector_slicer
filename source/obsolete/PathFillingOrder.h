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
