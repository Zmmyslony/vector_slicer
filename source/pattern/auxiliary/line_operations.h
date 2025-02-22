// Copyright (c) 2023-2025, Michał Zmyślony, mlz22@cam.ac.uk.
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
// Created by Michał Zmyślony on 19/06/2023.
//

#ifndef VECTOR_SLICER_LINE_OPERATIONS_H
#define VECTOR_SLICER_LINE_OPERATIONS_H

#include <vector>
#include <tuple>
#include "../coord.h"

using vecd = std::vector<double>;
using veci = std::vector<int>;

std::vector<std::vector<veci>> separateLines(std::vector<veci> &sorted_perimeters, double separation_distance);

std::vector<std::vector<coord>>
separateIntoLines(std::vector<coord> &unsorted_perimeters, coord starting_coordinates, double separation_distance);

bool isLooped(const std::vector<coord> &line);

std::vector<coord> pixeliseLine(const coord_d &line);

#endif //VECTOR_SLICER_LINE_OPERATIONS_H
