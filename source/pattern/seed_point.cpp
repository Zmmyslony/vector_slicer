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

#include "seed_point.h"
#include "coord.h"

#include <utility>
#include <stdexcept>

SeedPoint::SeedPoint() = default;

SeedPoint::SeedPoint(coord coordinates, coord_d director, int seed_line, int index) :
        coordinates(std::move(coordinates)),
        seed_line(seed_line),
        index(index),
        director(std::move(director)) {}


const coord & SeedPoint::getCoordinates() const {
    return coordinates;
}

int SeedPoint::getSeedLine() const {
    return seed_line;
}

int SeedPoint::getIndex() const {
    return index;
}

bool SeedPoint::isInvalid() const {
    if (coordinates.x == -1 &&
        coordinates.y == -1) {
        return true;
    }
    return false;
}

const coord_d & SeedPoint::getDirector() const {
    return director;
}
