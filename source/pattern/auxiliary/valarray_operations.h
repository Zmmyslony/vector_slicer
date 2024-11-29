// Copyright (c) 2021-2023, Michał Zmyślony, mlz22@cam.ac.uk.
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
// Created by Michał Zmyślony on 15/10/2021.
//

#ifndef VECTOR_SLICER_VALARRAY_OPERATIONS_H
#define VECTOR_SLICER_VALARRAY_OPERATIONS_H

#include <vector>
#include "geometry.h"

using vecd = std::vector<double>;
using veci = std::vector<int>;

using matrix_d = std::vector<vecd>;

vecd itod(const veci &int_array);

veci dtoi(const vecd &double_array);

double generalNorm(const vecd &array, const double &exponent);

double norm(const vecd &array);

double norm(const veci &array);

vecd generalNormalize(const vecd &array, const double &exponent);

vecd normalize(const vecd &array);

vecd normalize(const veci &array);

double dot(const vecd &array_first, const vecd &array_second);

vecd perpendicular(const vecd &vector);

veci perpendicular(const veci &vector);

double cross(const vecd &vector_first, const vecd &vector_second);

void printArray(const std::vector<veci> &array);

void printArray(const std::vector<vecd> &array);

double angle(const coord_d &array_first, const coord_d &array_second);

coord_d matrix_multiply(matrix_d mat, coord_d vec);

matrix_d tensor(coord_d vec_first, coord_d vec_second);

#endif //VECTOR_SLICER_VALARRAY_OPERATIONS_H
