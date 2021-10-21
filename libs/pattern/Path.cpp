//
// Created by Michał Zmyślony on 21/09/2021.
//

#include "Path.h"

void Path::addPointToForwardArray(std::valarray<int>& positions) {
    positionsAlongDirector.push_back(positions);
}

void Path::addPointToReverseArray(std::valarray<int>& positions) {
    positionsOppositeToDirector.push_back(positions);
}

Path::Path(std::vector<std::valarray<int>>& forwardPath, std::vector<std::valarray<int>>& backwardPath):
    positionsAlongDirector(forwardPath),
    positionsOppositeToDirector(backwardPath)
{ }

Path::Path(std::valarray<int>& startingPositions) {
    addPointToForwardArray(startingPositions);
    addPointToReverseArray(startingPositions);
}