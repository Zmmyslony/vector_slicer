//
// Created by Michał Zmyślony on 22/09/2021.
//

#include "PerimeterChecking.h"
#include "ValarrayConversion.h"

std::valarray<double> getRepulsion(std::vector<std::vector<int>> &filledTable,
                                   std::vector<std::valarray<int>> &checkedArea,
                                   std::valarray<int> &startPositions,
                                   std::valarray<int> &sizes, double repulsionCoefficient) {

    std::valarray<double> repulsion = {0, 0};
    for (auto &direction: checkedArea) {
        std::valarray<int> positionsNew = direction + startPositions;
        if (0 <= positionsNew[0] && positionsNew[0] < sizes[0] && 0 <= positionsNew[1] && positionsNew[1] < sizes[1]) {
            if (filledTable[positionsNew[0]][positionsNew[1]] > 0) {
                repulsion += itodArray(direction);
            }
        }
    }

    return repulsionCoefficient * repulsion / checkedArea.size();
}


bool isPerimeterFree(std::vector<std::vector<int>> &filledTable, std::vector<std::vector<int>> &shapeTable,
                     std::vector<std::valarray<int>> &perimeterList,
                     std::valarray<int> &startPositions, std::valarray<int> &sizes) {
    if (shapeTable[startPositions[0]][startPositions[1]] == 0) {
        return false;
    }
    for (auto &perimeter: perimeterList) {
        std::valarray<int> positionsNew = perimeter + startPositions;
        if (0 <= positionsNew[0] && positionsNew[0] < sizes[0] && 0 <= positionsNew[1] && positionsNew[1] < sizes[1]) {
            if (filledTable[positionsNew[0]][positionsNew[1]] > 0) {
                return false;
            }
        }
    }
    return true;
}


bool
isOnEdge(std::vector<std::vector<int>> &shapeTable, std::valarray<int> &startPositions, std::valarray<int> &sizes) {
    std::vector<std::valarray<int>> listOfNearestNeighbours = {{-1, 0},
                                                               {-1, 1},
                                                               {0,  1},
                                                               {1,  1},
                                                               {1,  0},
                                                               {1,  -1},
                                                               {0,  -1},
                                                               {-1, -1}};

    if (shapeTable[startPositions[0]][startPositions[1]] == 0) {
        return false;
    }

    for (auto &neighbour: listOfNearestNeighbours) {
        std::valarray<int> positionsNew = neighbour + startPositions;
        if (0 <= positionsNew[0] && positionsNew[0] < sizes[0] && 0 <= positionsNew[1] && positionsNew[1] < sizes[1]) {
            if (shapeTable[positionsNew[0]][positionsNew[1]] == 0) {
                return true;
            }
        }
    }
    return false;
}