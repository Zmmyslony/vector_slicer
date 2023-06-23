// Copyright (c) 2022-2023, Michał Zmyślony, mlz22@cam.ac.uk.
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
// Created by Michał Zmyślony on 21/11/2022.
//

#ifndef VECTOR_SLICER_VECTOR_OPERATIONS_H
#define VECTOR_SLICER_VECTOR_OPERATIONS_H

#include <valarray>
#include <vector>

using vald = std::valarray<double>;
using vali = std::valarray<int>;

using vecd = std::vector<double>;
using veci = std::vector<int>;

std::vector<vali> stitchTwoVectors(std::vector<vali> backwards_vector, std::vector<vali> forwards_vector);

double mean(const vecd &data);

double standardDeviation(const vecd &data);

vecd itod(const std::vector<int> &vector);

std::vector<int> dtoi(const vecd &vector);

vecd add(const vecd &first, const vecd &second);

veci add(const veci &first, const veci &second);

double dot(const vecd &first, const vecd &second);

double norm(const vecd& vec);

vecd scale(const vecd &vec, double scale);

vecd normalize(const vecd &vec);

vali vectoval(const veci &vec);

vald vectoval(const vecd &vec);

veci valtovec(const vali &vec);

vecd valtovec(const vald &vec);


#endif //VECTOR_SLICER_VECTOR_OPERATIONS_H
