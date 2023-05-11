// Copyright (c) 2021-2023, Michał Zmyślony, mlz22@cam.ac.uk.
//
// Please cite Michał Zmyślony and Dr John Biggins if you use any part of this code in work you publish or distribute.
//
// This file is part of Vector Slicer.
//
// Vector Slicer is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
// later version.
//
// Vector Slicer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License along with Vector Slicer.
// If not, see <https://www.gnu.org/licenses/>.

//
// Created by Michał Zmyślony on 21/09/2021.
//

#include "path.h"
#include "auxiliary/valarray_operations.h"

void Path::addPoint(vali &positions) {
    sequence_of_positions.push_back(positions);
}


Path::Path(vali &starting_positions) {
    addPoint(starting_positions);
}


unsigned int Path::size() const {
    return sequence_of_positions.size();
}


Path::Path(const Path &forward_path, const Path &backward_path) {
    std::vector<vali> backward_sequence = backward_path.sequence_of_positions;
    std::vector<vali> forward_sequence = forward_path.sequence_of_positions;

    std::reverse(backward_sequence.begin(), backward_sequence.end());
    backward_sequence.insert(backward_sequence.end(), forward_sequence.begin() + 1, forward_sequence.end());
    sequence_of_positions = backward_sequence;
}

vali Path::first() const {
    return sequence_of_positions.front();
}

vali Path::second() const {
    return sequence_of_positions[1];
}

vali Path::last() const {
    return sequence_of_positions.back();
}

vali Path::secondToLast() const {
    return sequence_of_positions[sequence_of_positions.size() - 2];
}

double Path::getLength() {
    double length = 0;
    for (int i = 1; i < size(); i++) {
        length += norm(sequence_of_positions[i] - sequence_of_positions[i - 1]);
    }
    return length;
}
