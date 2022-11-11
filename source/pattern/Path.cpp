//
// Created by Michał Zmyślony on 21/09/2021.
//

#include "Path.h"

void Path::addPoint(std::valarray<int> &positions) {
    sequenceOfPositions.push_back(positions);
}


Path::Path(std::valarray<int> &startingPositions) {
    addPoint(startingPositions);

}


unsigned int Path::getSize() const {
    return sequenceOfPositions.size();
}


Path::Path(Path forwardPath, Path backwardPath) {
    for (int i = 0; i < backwardPath.getSize(); i++) {
        sequenceOfPositions.push_back(backwardPath.sequenceOfPositions[backwardPath.getSize() - i - 1]);
    }
    for (int i = 1; i < forwardPath.getSize(); i++) {
        sequenceOfPositions.push_back(forwardPath.sequenceOfPositions[i]);
    }
}

std::valarray<int> Path::first() {
    return sequenceOfPositions[0];
}

std::valarray<int> Path::second() {
    return sequenceOfPositions[1];
}

std::valarray<int> Path::last() {
    return sequenceOfPositions.back();
}

std::valarray<int> Path::previousToLast() {
    return sequenceOfPositions[sequenceOfPositions.size() - 2];
}
