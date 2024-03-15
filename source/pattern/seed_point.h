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
// Created by Michał Zmyślony on 11/03/2024.
//

#ifndef VECTOR_SLICER_SEED_POINT_H
#define VECTOR_SLICER_SEED_POINT_H

#include <vector>
#include <valarray>

using vali = std::valarray<int>;
using vald = std::valarray<double>;

class SeedPoint {
    vali coordinates;
    vald director;
    int seed_line{};
    int index{};

public:
    SeedPoint();

    SeedPoint(vali coordinates, vald director, int seed_line, int index);

    const vali &getCoordinates() const;

    int getSeedLine() const;

    int getIndex() const;

    bool isInvalid() const;

    const vald &getDirector() const;

};

/// Seed point representing invalidity.
const SeedPoint INVALID_SEED = {{-1, -1}, {-1, -1}, -1, -1};

#endif //VECTOR_SLICER_SEED_POINT_H
