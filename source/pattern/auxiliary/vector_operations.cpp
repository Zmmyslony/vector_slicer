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

#include "vector_operations.h"

#include <numeric>
#include <stdexcept>

#include "valarray_operations.h"

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

double mean(const std::vector<double> &data) {
    double sum = std::accumulate(std::begin(data), std::end(data), 0.0);
    double mean = sum / data.size();
    return mean;
}

double standardDeviation(const std::vector<double> &data) {
    double mean_value = mean(data);
    long double variance_sum = 0;
    for (auto &element: data) {
        long double difference = element - mean_value;
        variance_sum += difference * difference;
    }
    auto standard_deviation = (double) (sqrt(variance_sum) / data.size());
    return standard_deviation;
}

std::vector<double> itod(const std::vector<int> &vector) {
    return {vector.begin(), vector.end()};
}

std::vector<int> dtoi(const std::vector<double> &vector) {
    return {vector.begin(), vector.end()};
}

std::vector<double> add(const std::vector<double> &first, const std::vector<double> &second) {
    if (first.size() == second.size()) {
        std::vector<double> sum = first;
        for (int i = 0; i < first.size(); i++) {
            sum[i] += second[i];
        }
        return sum;
    } else {
        throw std::runtime_error("Adding two vectors of uneven size");
    }
}

vali vectoval(const veci &vec) {
    return {vec.data(), vec.size()};
}

vald vectoval(const vecd &vec) {
    return {vec.data(), vec.size()};
}

double dot(const vecd &first, const vecd &second) {
    return dot(vectoval(first), vectoval(second));
}

double norm(const vecd &vec) {
    return norm(vectoval(vec));
}

vecd scale(const vecd &vec, double scale) {
    vecd vec_rescaled;
    for (auto &element: vec) {
        vec_rescaled.emplace_back(element * scale);
    }
    return vec_rescaled;
}

vecd normalize(const vecd &vec) {
    double vec_norm = norm(vec);
    if (vec_norm > 0) {
        return scale(vec, 1 / vec_norm);
    } else {
        return vec;
    }
}

veci valtovac(const vali &vec) {
    return {std::begin(vec), std::end(vec)};
}

vecd valtovac(const vald &vec) {
    return {std::begin(vec), std::end(vec)};
}

