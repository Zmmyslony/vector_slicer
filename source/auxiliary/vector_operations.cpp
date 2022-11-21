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
// You should have received a copy of the GNU General Public License along with Foobar. If not, see <https://www.gnu.org/licenses/>.

//
// Created by Michał Zmyślony on 21/11/2022.
//

#include "vector_operations.h"

std::vector<std::valarray<int>>
stitchTwoVectors(std::vector<std::valarray<int>> backwards_vector, std::vector<std::valarray<int>> forwards_vector) {
    std::reverse(backwards_vector.begin(), backwards_vector.end());
    if (!backwards_vector.empty()) {
        backwards_vector.pop_back();
    }
    std::vector<std::valarray<int>> stitched_vector = backwards_vector;
    stitched_vector.reserve(backwards_vector.size() + forwards_vector.size());
    stitched_vector.insert(stitched_vector.end(), forwards_vector.begin(), forwards_vector.end());
    return stitched_vector;
}


