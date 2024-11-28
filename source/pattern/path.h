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
#include "seed_point.h"

using veci = std::vector<int>;
using vecd = std::vector<double>;

/// Contains the sequence of coordinates that form a single path
class Path {
    SeedPoint seed_point;
    std::vector<veci> sequence_of_positions;
    std::vector<double> overlap;
    std::vector<vecd> positive_path_edge;
    std::vector<vecd> negative_path_edge;

    bool is_reversed = false;
    /// Distance between the point and the start of the path.
    double vectorDistance(const veci &point) const;
    /// Shorter distance between the point and the start of the path, which reverses the path when needed.
    double tensorDistance(const veci &point);
public:

    explicit Path(SeedPoint seed, double print_radius);

    Path(const Path& forward_path, const Path& backward_path);

    void addPoint(const veci &positions, double segment_overlap, const vecd &positive_edge, const vecd &negative_edge);

    void addPoint(const veci &positions, const vecd &positive_edge, const vecd &negative_edge);

    const std::vector<vecd> &getPositivePathEdge() const;

    const std::vector<vecd> &getNegativePathEdge() const;

    [[nodiscard]] veci first() const ;

    [[nodiscard]] veci last() const ;

    [[nodiscard]] unsigned int size() const;

    [[nodiscard]] veci secondToLast() const;

    [[nodiscard]] veci second() const;

    double getLength();

    const SeedPoint &getSeedPoint() const;

    /// Vector or tensor distance
    double distance(const veci& point, bool is_vector_filled);

    /// Last point taking into account the directionality
    [[nodiscard]] veci endPoint() const;

    /// Returns sequence of positions in forward or backward order depending on isReversed
    std::vector<veci> getPositionSequence() const;

    /// Return position at index
    veci position(unsigned int index);

    std::vector<double> getOverlap() const;

    void setOverlap(const std::vector<double> &overlap);

    bool isReversed() const;

    std::vector<veci> findPointsToFill(int i, bool is_position_filled) const;

    std::vector<veci> findPointsToFill(bool is_position_filled) const;
};


#endif //VECTOR_SLICER_PATH_H
