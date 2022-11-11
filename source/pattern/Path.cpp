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
