// Copyright (c) 2024-2025, Michał Zmyślony, mlz22@cam.ac.uk.
//
// Please cite following publication if you use any part of this code in work you publish or distribute:
// [1] Michał Zmyślony M., Klaudia Dradrach, John S. Biggins,
//    Slicing vector fields into tool paths for additive manufacturing of nematic elastomers,
//    Additive Manufacturing, Volume 97, 2025, 104604, ISSN 2214-8604, https://doi.org/10.1016/j.addma.2024.104604.
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


coord_d::coord_d() : x(0), y(0) {}

coord_d::coord_d(double x_val, double y_val) : x(x_val), y(y_val) {}

coord_d coord_d::operator+(const coord_d &other) const {
    return {x + other.x, y + other.y};
}

coord_d coord_d::operator-(const coord_d &other) const {
    return {x - other.x, y - other.y};
}

void coord_d::operator+=(const coord_d &other) {
    x += other.x;
    y += other.y;
}

void coord_d::operator-=(const coord_d &other) {
    *this = *this - other;
}

coord_d coord_d::operator*(double multiplier) const {
    return {x * multiplier, y * multiplier};
}

void coord_d::operator*=(double multiplier) {
    x *= multiplier;
    y *= multiplier;
}

coord_d coord_d::operator/(double divisor) const {
    return {x / divisor, y / divisor};
}

void coord_d::operator/=(double divisor) {
    *this = *this / divisor;
}

bool coord_d::operator==(const coord_d &other) const {
    return (x == other.x) && (y == other.y);
}

double coord_d::norm() const {
    return sqrt(x * x + y * y);
}

coord_d coord_d::normalized() const {
    return *this / norm();
}

void coord_d::normalize() {
    *this = normalized();
}

coord::coord() : x(-1), y(-1) {}

coord::coord(short x_val, short y_val) : x(x_val), y(y_val) {}

coord coord::operator+(const coord &other) const {
    return {static_cast<short>(x + other.x), static_cast<short>(y + other.y)};
}

coord coord::operator-(const coord &other) const {
    return {static_cast<short>(x - other.x), static_cast<short>(y - other.y)};
}

void coord::operator+=(const coord &other) {
    *this = *this + other;
}

void coord::operator-=(const coord &other) {
    *this = *this - other;
}

coord coord::operator*(int multiplier) const {
    return {static_cast<short>(x * multiplier), static_cast<short>(y * multiplier)};
}

void coord::operator*=(int multiplier) {
    *this = *this * multiplier;
}

double coord::norm() const {
    return sqrt(x * x + y * y);
}

coord_d coord::normalized() const {
    return coord_d((double) x, (double) y) / norm();
}

bool coord::operator==(const coord &other) const {
    return (x == other.x) && (y == other.y);
}

coord::coord(const coord_d &other) {
    x = (short) lround(other.x);
    y = (short) lround(other.y);
}

coord_d coord::operator*(double multiplier) const {
    return {(double) x * multiplier, (double) y * multiplier};
}

coord_d coord::operator+(const coord_d &other) const {
    return {(double) x + other.x, (double) y + other.y};
}

coord_d coord::operator-(const coord_d &other) const {
    return {(double) x - other.x, (double) y - other.y};
}

coord::coord(int x_val, int y_val) : x((short)x_val), y((short)y_val) {}

bool coord::operator!=(const coord &other) const {
    return !(*this == other);
}

double dot(const coord_d &lhs, const coord_d &rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

double dot(const coord &lhs, const coord &rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

double cross(const coord_d &lhs, const coord_d &rhs) {
    return lhs.x * rhs.y - lhs.y * rhs.x;
}

double cross(const coord &lhs, const coord &rhs) {
    return lhs.x * rhs.y - lhs.y * rhs.x;
}

coord perpendicular(const coord &point) {
    return {static_cast<short>(-point.y), point.x};
}

coord_d perpendicular(const coord_d &point) {
    return {-point.y, point.x};
}

coord_d to_coord_d(const coord &other) {
    return {(double)other.x, (double)other.y};
}





