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
#include <stdexcept>
#include "auxiliary/valarray_operations.h"
#include "auxiliary/geometry.h"

void
Path::addPoint(const vali &positions, double segment_overlap, const vald &positive_edge, const vald &negative_edge) {
    sequence_of_positions.push_back(positions);
    overlap.push_back(segment_overlap);
    negative_path_edge.push_back(negative_edge);
    positive_path_edge.push_back(positive_edge);
}

void Path::addPoint(const vali &positions, const vald &positive_edge, const vald &negative_edge) {
    addPoint(positions, 0, positive_edge, negative_edge);
}


Path::Path(SeedPoint seed, double print_radius) : seed_point(std::move(seed)){
    vald positions = itod(seed_point.getCoordinates());
    vald tangent = seed_point.getDirector();
    vald normal = perpendicular(tangent) * print_radius;
    addPoint(seed_point.getCoordinates(),positions + normal, positions - normal);
}


unsigned int Path::size() const {
    return sequence_of_positions.size();
}

template <typename T>

std::vector<T> joinVectors(std::vector<T> forward_vector, std::vector<T> backward_vector) {
    std::reverse(backward_vector.begin(), backward_vector.end());
    backward_vector.insert(backward_vector.end(), forward_vector.begin() + 1, forward_vector.end());
    return backward_vector;
}


/// Joins two paths, reversing the backward path and appending to it the forward path.
Path::Path(const Path &forward_path, const Path &backward_path) {
    seed_point = forward_path.seed_point;
    sequence_of_positions = joinVectors(backward_path.sequence_of_positions, forward_path.sequence_of_positions);
    overlap = joinVectors(backward_path.overlap, forward_path.overlap);
    positive_path_edge = joinVectors(backward_path.positive_path_edge, forward_path.positive_path_edge);
    negative_path_edge = joinVectors(backward_path.negative_path_edge, forward_path.negative_path_edge);
}

/// Returns the first position in the path.
vali Path::first() const {
    return sequence_of_positions.front();
}

/// Returns the second position in the path.
vali Path::second() const {
    return sequence_of_positions[1];
}

/// Returns last position in the path.
vali Path::last() const {
    return sequence_of_positions.back();
}

/// Returns second to last position in the path.
vali Path::secondToLast() const {
    return sequence_of_positions[sequence_of_positions.size() - 2];
}

/// Returns total length of the path.
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

/// Returns distance between the point and the first() point.
double Path::vectorDistance(const vali &point) const {
    return norm(point - first());
}

/// Returns shorter distance between point and  first() or last(). If distance from last() is closer, marks path as
/// reversed.
double Path::tensorDistance(const vali &point) {
    double forward_distance = norm(point - first());
    double backward_distance = norm(point - last());
    if (backward_distance < forward_distance) {
        is_reversed = true;
        return backward_distance;
    } else {
        is_reversed = false;
        return forward_distance;
    }
}


/// Last point taking into the account whether path is reversed.
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


/// Returns sequence of overlaps, taking into the account if path is reversed.
std::vector<double> Path::getOverlap() const {
    if (is_reversed) {
        std::vector<double> reversed_overlap = overlap;
        std::reverse(reversed_overlap.begin(), reversed_overlap.end());
        return reversed_overlap;
    } else {
        return overlap;
    }
}

void Path::setOverlap(const std::vector<double> &overlap) {
    if (overlap.size() != sequence_of_positions.size()) {
        throw std::runtime_error("Size of overlap array disagrees with path length.");
    }
    Path::overlap = overlap;
}

bool Path::isReversed() const {
    return is_reversed;
}

std::vector<vali> Path::findPointsToFill(int i, bool is_position_filled) const {
    return ::findPointsToFill(
            positive_path_edge[i - 1],
            negative_path_edge[i - 1],
            negative_path_edge[i],
            positive_path_edge[i],
            is_position_filled);
}

std::vector<vali> Path::findPointsToFill(bool is_position_filled) const {
    return findPointsToFill(size() - 1, is_position_filled);
}



