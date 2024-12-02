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
#include "coord.h"

using veci = std::vector<int>;
using vecd = std::vector<double>;

/// Contains the sequence of coordinates that form a single path
class Path {
    SeedPoint seed_point;
    std::vector<double> overlap;
    std::vector<coord_d> sequence_of_positions;
    std::vector<coord_d> positive_path_edge;
    std::vector<coord_d> negative_path_edge;

    bool is_reversed = false;
    /// Distance between the point and the start of the path.
    double vectorDistance(const coord_d &point) const;
    /// Shorter distance between the point and the start of the path, which reverses the path when needed.
    double tensorDistance(const coord_d &point);
public:

    explicit Path(SeedPoint seed, double print_radius);

    Path(const Path& forward_path, const Path& backward_path);

    void addPoint(const coord_d &positions, double segment_overlap, const coord_d &positive_edge, const coord_d &negative_edge);

    void addPoint(const coord_d &positions, const coord_d &positive_edge, const coord_d &negative_edge);

    const std::vector<coord_d> & getPositivePathEdge() const;

    const std::vector<coord_d> & getNegativePathEdge() const;

    [[nodiscard]] coord_d first() const ;

    [[nodiscard]] coord_d last() const ;

    [[nodiscard]] unsigned int size() const;

    [[nodiscard]] coord_d secondToLast() const;

    [[nodiscard]] coord_d second() const;

    double getLength();

    const SeedPoint &getSeedPoint() const;

    /// Vector or tensor distance
    double distance(const coord_d &point, bool is_vector_filled);

    /// Last point taking into account the directionality
    [[nodiscard]] coord_d endPoint() const;

    /// Returns sequence of positions in forward or backward order depending on isReversed
    std::vector<coord_d> getPositionSequence() const;

    std::vector<coord> getCoordinateSequence() const;

    /// Return position at index
    coord_d position(unsigned int index);

    std::vector<double> getOverlap() const;

    void setOverlap(const std::vector<double> &overlap);

    bool isReversed() const;

    std::vector<coord> findPointsToFill(int i, bool is_position_filled) const;

    std::vector<coord> findPointsToFill(bool is_position_filled) const;
};


#endif //VECTOR_SLICER_PATH_H
