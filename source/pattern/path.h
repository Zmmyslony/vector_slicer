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

#ifndef VECTOR_SLICER_PATH_H
#define VECTOR_SLICER_PATH_H

#include <vector>
#include <valarray>
#include "seed_point.h"

using vali = std::valarray<int>;

/// Contains the sequence of coordinates that form a single path
class Path {
    SeedPoint seed_point;
    std::vector<vali> sequence_of_positions;
    std::vector<double> overlap;

    bool is_reversed = false;
    /// Distance between the point and the start of the path.
    double vectorDistance(const vali &point) const;
    /// Shorter distance between the point and the start of the path, which reverses the path when needed.
    double tensorDistance(const vali &point);
public:

    explicit Path(SeedPoint seed);

    Path(const Path& forward_path, const Path& backward_path);

    void addPoint(const vali &positions, double segment_overlap);

    void addPoint(const vali &positions);

    [[nodiscard]] vali first() const ;

    [[nodiscard]] vali last() const ;

    [[nodiscard]] unsigned int size() const;

    [[nodiscard]] vali secondToLast() const;

    [[nodiscard]] vali second() const;

    double getLength();

    const SeedPoint &getSeedPoint() const;

    /// Vector or tensor distance
    double distance(const vali& point, bool is_vector_filled);

    /// Last point taking into account the directionality
    [[nodiscard]] vali endPoint() const;

    /// Returns sequence of positions in forward or backward order depending on isReversed
    std::vector<vali> getPositionSequence() const;

    /// Return position at index
    vali position(unsigned int index);

    std::vector<double> getOverlap() const;

    void setOverlap(const std::vector<double> &overlap);

    bool isReversed() const;
};


#endif //VECTOR_SLICER_PATH_H
