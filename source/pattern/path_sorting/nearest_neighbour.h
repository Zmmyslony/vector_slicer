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
// Created by Michał Zmyślony on 12/03/2024.
//

#ifndef VECTOR_SLICER_NEAREST_NEIGHBOUR_H
#define VECTOR_SLICER_NEAREST_NEIGHBOUR_H

#include <vector>
#include "../path.h"
#include "../filled_pattern.h"

using veci = std::vector<int>;


std::vector<Path>
nearestNeighbourSort(const FilledPattern &pattern, const coord &starting_coordinates);

#endif //VECTOR_SLICER_NEAREST_NEIGHBOUR_H
