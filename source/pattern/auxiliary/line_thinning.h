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
// Created by Michał Zmyślony on 23/06/2023.
//

#ifndef VECTOR_SLICER_LINE_THINNING_H
#define VECTOR_SLICER_LINE_THINNING_H

#include <vector>
#include <set>
#include <unordered_set>
#include <boost/functional/hash.hpp>
#include "../coord.h"

/// Zhang-Suen line thinning algorithm
coord_set skeletonize(coord_set shape, int grow_size, const std::vector<std::vector<uint8_t>> &shape_matrix);

coord_set grow_pattern(const coord_set &shape, double radius, const std::vector<std::vector<uint8_t>> &shape_matrix);


#endif //VECTOR_SLICER_LINE_THINNING_H
