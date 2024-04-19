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

#ifndef VECTOR_SLICER_POSITION_H
#define VECTOR_SLICER_POSITION_H


/// Pair of double positions x and y
struct Position {
    double x = -1;
    double y = -1;

    explicit Position(double x, double y) : x(x), y(y) {};

    Position operator+(const Position &rhs) const {
        return Position(x + rhs.x, y + rhs.y);
    }

    Position operator-(const Position &rhs) const {
        return Position(x - rhs.x, y - rhs.y);
    }

    void operator+=(const Position &rhs) {
        *this = *this + rhs;
    }

    void operator-=(const Position &rhs) {
        *this = *this - rhs;
    }

    Position operator*(double multiplier) const {
        return Position(x * multiplier, y * multiplier);
    }

    void operator*=(double multiplier) {
        *this = *this * multiplier;
    }

    Position operator/(double divisor) const {
        return *this * (1 / divisor);
    }

    void operator/=(double multiplier) {
        *this = *this / multiplier;
    }
};


#endif //VECTOR_SLICER_POSITION_H
