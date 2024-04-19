// Copyright (c) 2024, Michał Zmyślony, mlz22@cam.ac.uk.
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
// Created by Michał Zmyślony on 18/04/2024.
//

#ifndef VECTOR_SLICER_COORD_H
#define VECTOR_SLICER_COORD_H

#include <vector>
#include <set>
#include <unordered_set>
#include <boost/functional/hash.hpp>

using coord = std::pair<uint16_t, uint16_t>;

struct pairhash {
public:
    uint32_t operator()(const std::pair<uint16_t, uint16_t> &x) const {
        return x.first * 65536 + x.second;
    }
};

using coord_set = std::unordered_set<coord, pairhash>;
using coord_sequence = std::set<coord>;
using coord_vector = std::vector<coord>;

inline coord operator+(const coord &lhs, const coord &rhs) {
    return {lhs.first + rhs.first, lhs.second + rhs.second};
}

inline void operator+=(coord &lhs, const coord &rhs) {
    lhs = lhs + rhs;
}

inline coord operator-(const coord &lhs, const coord &rhs) {
    return {lhs.first - rhs.first, lhs.second - rhs.second};
}

inline void operator-=(coord &lhs, const coord &rhs) {
    lhs = lhs - rhs;
}

inline coord operator*(const coord &lhs, double multiplier) {
    return {lhs.first * multiplier, lhs.second * multiplier};
}

inline void operator*=(coord &lhs, double multiplier) {
    lhs = lhs * multiplier;
}

inline coord operator/(const coord &lhs, double divisor) {
    return lhs * (1 / divisor);
}

inline void operator/=(coord &lhs, double divisor) {
    lhs = lhs / divisor;
}

#endif //VECTOR_SLICER_COORD_H
