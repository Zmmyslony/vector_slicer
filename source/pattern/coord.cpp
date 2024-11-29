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
// Created by Michał Zmyślony on 29/11/2024.
//

#include "coord.h"
#include <cmath>

double norm(const coord_d &point) {
    return sqrt(point.first * point.first + point.second * point.second);
}

double norm(const coord &point) {
    return sqrt(point.first * point.first + point.second * point.second);
}

void normalize(coord_d &point) {
    point /= norm(point);
}

coord_d normalized(const coord_d &point) {
    return point / norm(point);
}

coord_d normalized(const coord &point) {
    double norm_val = norm(point);
    return {(double)point.first / norm_val, (double)point.second / norm_val};
}


double dot(const coord_d &lhs, const coord_d &rhs) {
    return lhs.first * rhs.first + lhs.second * rhs.second;
}

double dot(const coord &lhs, const coord &rhs) {
    return lhs.first * rhs.first + lhs.second * rhs.second;
}

double cross(const coord_d &lhs, const coord_d &rhs) {
    return lhs.first * rhs.second - lhs.second * rhs.first;
}

double cross(const coord &lhs, const coord &rhs) {
    return lhs.first * rhs.second - lhs.second * rhs.first;
}

coord perpendicular(const coord &point) {
    return {-point.second, point.first};
}

coord_d perpendicular(const coord_d &point) {
    return {-point.second, point.first};
}