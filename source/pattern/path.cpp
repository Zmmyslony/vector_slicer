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

#include <utility>
#include "auxiliary/valarray_operations.h"

void Path::addPoint(const vali &positions, double segment_overlap) {
    sequence_of_positions.push_back(positions);
    overlap.push_back(segment_overlap);
}

void Path::addPoint(const vali &positions) {
    addPoint(positions, 0);
}


Path::Path(SeedPoint seed) : seed_point(std::move(seed)){
    addPoint(seed_point.getCoordinates());
}


unsigned int Path::size() const {
    return sequence_of_positions.size();
}


Path::Path(const Path &forward_path, const Path &backward_path) {
    std::vector<vali> backward_sequence = backward_path.sequence_of_positions;
    std::vector<vali> forward_sequence = forward_path.sequence_of_positions;

    std::vector<double> backward_overlap = backward_path.overlap;
    std::vector<double> forward_overlap = forward_path.overlap;

    std::reverse(backward_sequence.begin(), backward_sequence.end());
    std::reverse(backward_overlap.begin(), backward_overlap.end());
    backward_sequence.insert(backward_sequence.end(), forward_sequence.begin() + 1, forward_sequence.end());
    backward_overlap.insert(backward_overlap.end(), forward_overlap.begin() + 1, forward_overlap.end());
    sequence_of_positions = backward_sequence;
    overlap = backward_overlap;
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

const SeedPoint &Path::getSeedPoint() const {
    return seed_point;
}

double Path::vectorDistance(const vali &point) const {
    return norm(point - first());
}

double Path::tensorDistance(const vali &point) {
    double forward_distance = norm(point - first());
    double backward_distance = norm(point - last());
    if (backward_distance < forward_distance) {
        is_reversed = true;
        return backward_distance;
    }
    return forward_distance;
}

vali Path::endPoint() const {
    if (is_reversed) {
        return first();
    } else {
        return last();
    }
}

double Path::distance(const vali &point, bool is_vector_filled) {
    if (is_vector_filled) {
        return vectorDistance(point);
    } else {
        return tensorDistance(point);
    }
}

std::vector<vali> Path::getPositionSequence() const {
    if (is_reversed) {
        std::vector<vali> position_sequence = sequence_of_positions;
        std::reverse(position_sequence.begin(), position_sequence.end());
        return position_sequence;
    } else {
        return sequence_of_positions;
    }
}

vali Path::position(unsigned int index) {
    return sequence_of_positions[index];
}




