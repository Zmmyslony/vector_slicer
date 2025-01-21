// Copyright (c) 2022-2025, Michał Zmyślony, mlz22@cam.ac.uk.
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
// Created by Michał Zmyślony on 21/11/2022.
//

#include "vector_operations.h"

#include <numeric>
#include <stdexcept>
#include <cmath>

#include "valarray_operations.h"

double mean(const vecd &data) {
    double sum = std::accumulate(std::begin(data), std::end(data), 0.0);
    double mean = sum / data.size();
    return mean;
}

double standardDeviation(const vecd &data) {
    double mean_value = mean(data);
    long double variance_sum = 0;
    for (auto &element: data) {
        long double difference = element - mean_value;
        variance_sum += difference * difference;
    }
    auto standard_deviation = (double) (sqrt(variance_sum) / data.size());
    return standard_deviation;
}


vecd add(const vecd &first, const vecd &second) {
    if (first.size() == second.size()) {
        vecd sum = first;
        for (int i = 0; i < first.size(); i++) {
            sum[i] += second[i];
        }
        return sum;
    } else {
        throw std::runtime_error("Adding two vectors of uneven size");
    }
}

veci add(const veci &first, const veci &second) {
    if (first.size() == second.size()) {
        veci sum = first;
        for (int i = 0; i < first.size(); i++) {
            sum[i] += second[i];
        }
        return sum;
    } else {
        throw std::runtime_error("Adding two vectors of uneven size");
    }
}

vecd scale(const vecd &vec, double scale) {
    vecd vec_rescaled;
    for (auto &element: vec) {
        vec_rescaled.emplace_back(element * scale);
    }
    return vec_rescaled;
}


veci valtovec(const veci &vec) {
    return {std::begin(vec), std::end(vec)};
}

vecd valtovec(const vecd &vec) {
    return {std::begin(vec), std::end(vec)};
}

