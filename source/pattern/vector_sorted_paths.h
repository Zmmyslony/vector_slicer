// Copyright (c) 2023, Michał Zmyślony, mlz22@cam.ac.uk.
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
// Created by Michał Zmyślony on 03/04/2023.
//

#ifndef VECTOR_SLICER_VECTOR_SORTED_PATHS_H
#define VECTOR_SLICER_VECTOR_SORTED_PATHS_H

#include "path.h"
#include "auxiliary/valarray_operations.h"


std::vector<std::vector<vali>> getVectorSortedPaths(std::vector<Path> unsorted_paths, const vali& starting_point);



#endif //VECTOR_SLICER_VECTOR_SORTED_PATHS_H
