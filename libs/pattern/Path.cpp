//
// Created by Michał Zmyślony on 21/09/2021.
//

#include "Path.h"

void Path::addPoint(std::valarray<int>& positions) {
    sequenceOfPositions.push_back(positions);
}


Path::Path(std::valarray<int>& startingPositions) {
    addPoint(startingPositions);

}


unsigned int Path::getLength() const {
    return sequenceOfPositions.size();
}


Path::Path(Path forwardPath, Path backwardPath) {
    for (int i = 0; i < backwardPath.getLength(); i++) {
        sequenceOfPositions.push_back(backwardPath.sequenceOfPositions[backwardPath.getLength() - i - 1]);
    }
    for (int i = 1; i < forwardPath.getLength(); i++) {
        sequenceOfPositions.push_back(forwardPath.sequenceOfPositions[i]);
    }
}