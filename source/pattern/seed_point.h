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
// Created by Michał Zmyślony on 11/03/2024.
//

#ifndef VECTOR_SLICER_SEED_POINT_H
#define VECTOR_SLICER_SEED_POINT_H

#include <vector>
#include "coord.h"

using veci = std::vector<int>;
using vecd = std::vector<double>;

class SeedPoint {
    coord coordinates;
    coord_d director;
    int seed_line{};
    int index{};

public:
    SeedPoint();

    SeedPoint(coord coordinates, coord_d director, int seed_line, int index);

    const coord & getCoordinates() const;

    int getSeedLine() const;

    int getIndex() const;

    bool isInvalid() const;

    const coord_d & getDirector() const;

};

/// Seed point representing invalidity.
const SeedPoint INVALID_SEED = {{-1, -1}, {-1, -1}, -1, -1};

#endif //VECTOR_SLICER_SEED_POINT_H
