//
// Created by Michał Zmyślony on 21/09/2021.
//

#include "Path.h"
#include "../auxiliary/ValarrayOperations.h"

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

std::valarray<int> Path::last() {
    return sequenceOfPositions.back();
}

double Path::getLength() {
    double length = 0;
    for (int i = 1; i < getSize(); i++) {
        length += norm(sequenceOfPositions[i] - sequenceOfPositions[i - 1]);
    }
    return length;
}