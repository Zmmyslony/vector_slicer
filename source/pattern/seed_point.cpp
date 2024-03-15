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

#include "seed_point.h"

#include <utility>
#include <stdexcept>

SeedPoint::SeedPoint() = default;

SeedPoint::SeedPoint(vali coordinates, vald director, int seed_line, int index) :
        coordinates(std::move(coordinates)),
        seed_line(seed_line),
        index(index),
        director(std::move(director)) {}


const vali &SeedPoint::getCoordinates() const {
    return coordinates;
}

int SeedPoint::getSeedLine() const {
    return seed_line;
}

int SeedPoint::getIndex() const {
    return index;
}

bool SeedPoint::isInvalid() const {
    if (coordinates.size() == 0) {
        throw std::runtime_error("ERROR Seed point has empty coordinates.");
    }
    if (coordinates[0] == -1 &&
        coordinates[1] == -1) {
        return true;
    }
    return false;
}

const vald &SeedPoint::getDirector() const {
    return director;
}
