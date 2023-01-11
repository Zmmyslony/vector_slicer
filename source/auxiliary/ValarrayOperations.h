// 2022, Michał Zmyślony, mlz22@cam.ac.uk.
//
// Please cite Michał Zmyślony and Dr John Biggins if you use any part of this code in work you publish or distribute.
//
// This file is part of Vector Slicer.
//
// Vector Slicer is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//
// Vector Slicer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with Vector Slicer. If not, see <https://www.gnu.org/licenses/>.

//
// Created by Michał Zmyślony on 15/10/2021.
//

#ifndef VECTOR_SLICER_VALARRAYOPERATIONS_H
#define VECTOR_SLICER_VALARRAYOPERATIONS_H

#include <valarray>
#include <vector>

using vald = std::valarray<double>;
using vali = std::valarray<int>;

vald itod(const vali &int_array);

vali dtoi(const vald &double_array);

double generalNorm(const vald &array, const double &exponent);

double norm(const vald &array);

double norm(const vali &array);

vald generalNormalize(const vald &array, const double &exponent);

vald normalize(const vald &array);

vald normalize(const vali &array);

double dot(const vald &array_first, const vald &array_second);

vald perpendicular(vald vector);

double cross(const vald &vector_first, const vald &vector_second);

void printArray(const std::vector<vali> &array);

void printArray(const std::vector<vald> &array);

#endif //VECTOR_SLICER_VALARRAYOPERATIONS_H
