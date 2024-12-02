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

class coord_d {
public:
    double x;
    double y;

    coord_d();

    coord_d(double x, double y);

    coord_d operator+(const coord_d &other) const;

    coord_d operator-(const coord_d &other) const;

    void operator+=(const coord_d &other);

    void operator-=(const coord_d &other);

    coord_d operator*(double multiplier) const;

    void operator*=(double multiplier);

    coord_d operator/(double divisor) const;

    void operator/=(double divisor);

    bool operator==(const coord_d &other) const;

    [[nodiscard]] double norm() const;

    [[nodiscard]] coord_d normalized() const;

    void normalize();
};

double dot(const coord_d &lhs, const coord_d &rhs);

double cross(const coord_d &lhs, const coord_d &rhs);

coord_d perpendicular(const coord_d &point);

class coord {
public:
    short x;
    short y;

    coord();

    coord(short x, short y);

    coord(int x, int y);

    explicit coord(const coord_d &other);

    coord operator+(const coord &other) const;

    coord operator-(const coord &other) const;

    void operator+=(const coord &other);

    void operator-=(const coord &other);

    coord operator*(int multiplier) const;

    coord_d operator*(double multiplier) const;

    void operator*=(int multiplier);

    bool operator==(const coord &other) const;

    bool operator!=(const coord &other) const;

    coord_d operator+(const coord_d &other) const;

    coord_d operator-(const coord_d &other) const;

    [[nodiscard]] double norm() const;

    coord_d normalized() const;

};

double dot(const coord &lhs, const coord &rhs);

double cross(const coord &lhs, const coord &rhs);

coord perpendicular(const coord &point);

inline double norm(const coord &pt) { return pt.norm(); }

inline coord_d normalized(const coord &pt) { return pt.normalized(); }

inline double norm(const coord_d &pt) { return pt.norm(); }

inline coord_d normalized(const coord_d &pt) { return pt.normalized(); }

struct coord_hash {
public:
    unsigned int operator()(const coord &x) const {
        return ((int) x.x + 32768) * 65536 + ((int)x.y + 32768);
    }
};

using coord_set = std::unordered_set<coord, coord_hash>;
using coord_sequence = std::set<coord>;
using coord_vector = std::vector<coord>;

coord_d to_coord_d(const coord &other);

inline coord_d operator*(double multiplier, const coord_d &pt) { return pt * multiplier; }

inline coord_d operator+(coord_d second, const coord &first) {
    return {(double)first.x + second.x, (double)first.y + second.y};
}

#endif //VECTOR_SLICER_COORD_H
