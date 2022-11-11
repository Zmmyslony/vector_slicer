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
    sequence_of_positions.push_back(positions);
}


Path::Path(std::valarray<int> &starting_positions) {
    addPoint(starting_positions);

}


unsigned int Path::getSize() const {
    return sequence_of_positions.size();
}


Path::Path(Path forward_path, Path backward_path) {
    for (int i = 0; i < backward_path.getSize(); i++) {
        sequence_of_positions.push_back(backward_path.sequence_of_positions[backward_path.getSize() - i - 1]);
    }
    for (int i = 1; i < forward_path.getSize(); i++) {
        sequence_of_positions.push_back(forward_path.sequence_of_positions[i]);
    }
}

std::valarray<int> Path::first() {
    return sequence_of_positions[0];
}

std::valarray<int> Path::second() {
    return sequence_of_positions[1];
}

std::valarray<int> Path::last() {
    return sequence_of_positions.back();
}

std::valarray<int> Path::previousToLast() {
    return sequence_of_positions[sequence_of_positions.size() - 2];
}
