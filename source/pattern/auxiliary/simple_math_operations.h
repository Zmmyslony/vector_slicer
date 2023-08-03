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
// Created by Michał Zmyślony on 27/09/2021.
//

#ifndef VECTOR_SLICER_SIMPLE_MATH_OPERATIONS_H
#define VECTOR_SLICER_SIMPLE_MATH_OPERATIONS_H

#include <vector>
#include <valarray>

using vald = std::valarray<double>;


int roundUp(double);

int sgn(double number);

double decimalPart(double number);

std::vector<std::vector<std::valarray<double>>>
splayVector(const std::vector<std::vector<double>> &x_field, const std::vector<std::vector<double>> &y_field);

std::vector<std::vector<std::valarray<double>>> gradient(const std::vector<std::vector<double>> &field);

std::vector<std::vector<double>>
vectorArrayNorm(const std::vector<std::vector<std::valarray<double>>> &vector_array);

std::vector<std::vector<vald>> normalizeVectorArray(const std::vector<std::vector<vald>> &vector_array);

std::vector<std::vector<double>> divergence(const std::vector<std::vector<vald>> &field);

std::vector<int> findNullRows(const std::vector<std::vector<int>> &array);

std::vector<int> findNullColumns(const std::vector<std::vector<int>> &array);

template<typename T>
std::vector<std::vector<T>> removeRows(std::vector<std::vector<T>> &array, const std::vector<int> &rows_to_remove);

template<typename T>
std::vector<std::vector<T>>
removeColumns(std::vector<std::vector<T>> &array, const std::vector<int> &columns_to_remove);

template<typename T>
void adjustRowsAndColumns(std::vector<std::vector<T>> &array, const std::vector<int> &rows_to_remove,
                          const std::vector<int> &columns_to_remove);
#endif //VECTOR_SLICER_SIMPLE_MATH_OPERATIONS_H
