// Copyright (c) 2021-2025, Michał Zmyślony, mlz22@cam.ac.uk.
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
// Created by Michał Zmyślony on 23/09/2021.
//

#include <cmath>
#include <cassert>
#include <omp.h>
#include <algorithm>

#include "simple_math_operations.h"
#include "valarray_operations.h"

int roundUp(double number) {
    if (number > 0) {
        return (int) ceil(number);
    } else {
        return (int) floor(number);
    }
}

int sgn(double number) {
    return (number > 0) - (number < 0);
}

double decimalPart(double number) {
    return number - floor(number);
}

std::vector<std::vector<coord_d>>
joinTables(const std::vector<std::vector<double>> &x_field, const std::vector<std::vector<double>> &y_field,
           int threads) {
    assert((x_field.size() == y_field.size()));
    std::vector<std::vector<coord_d>> joined_table(x_field.size(), std::vector<coord_d>(x_field[0].size()));

    omp_set_num_threads(threads);
#pragma omp parallel for
    for (int i = 0; i < x_field.size(); i++) {
        assert((x_field[i].size() == y_field[i].size()));
        for (int j = 0; j < x_field[i].size(); j++) {
            joined_table[i][j] = {x_field[i][j], y_field[i][j]};
        }
    }
    return joined_table;
}

std::vector<std::vector<matrix_d>>
tensorWithItself(const std::vector<std::vector<coord_d>> &director, int threads) {
    std::vector<std::vector<matrix_d>> result(director.size(), std::vector<matrix_d>(director[0].size()));

    omp_set_num_threads(threads);
#pragma omp parallel for
    for (int i = 0; i < director.size(); i++) {
        for (int j = 0; j < director[i].size(); j++) {
            result[i][j] = tensor(director[i][j], director[i][j]);
        }
    }
    return result;
}


/// Calculation of the splay from the gradient theorem where Q = n (tensor) n, and splay is Q . Div(Q).b
std::vector<std::vector<coord_d>>
splayVector(const std::vector<std::vector<double>> &x_field, const std::vector<std::vector<double>> &y_field,
            int threads) {
    std::vector<std::vector<coord_d>> director = normalizeVectorArray(joinTables(x_field, y_field, threads), threads);
    std::vector<std::vector<matrix_d>> q_tensor = tensorWithItself(director, threads);

    std::vector<std::vector<coord_d>> splay_table(x_field.size(), std::vector<coord_d>(x_field[0].size()));
    // Temporarily use 0 splay values
    splay_table.front() = {x_field[0].size(), coord_d{0, 0}};
    splay_table.back() = {x_field[0].size(), coord_d{0, 0}};

    omp_set_num_threads(threads);
#pragma omp parallel for
    for (int i = 1; i < director.size() - 1; i++) {
        // Temporarily use 0 splay values
        splay_table[i].front() = {0, 0};
        splay_table[i].back() = {0, 0};
        for (int j = 1; j < director[i].size() - 1; j++) {
            coord_d q_divergence;
            q_divergence = matrix_multiply(q_tensor[i + 1][j], {1, 0});
            q_divergence +=  matrix_multiply(q_tensor[i][j + 1], {0, 1});
            q_divergence += matrix_multiply(q_tensor[i - 1][j], {-1, 0});
            q_divergence += matrix_multiply(q_tensor[i][j - 1], {0, -1});

            q_divergence += matrix_multiply(q_tensor[i + 1][j + 1], {0.5, 0.5});
            q_divergence += matrix_multiply(q_tensor[i - 1][j + 1], {-0.5, 0.5});
            q_divergence += matrix_multiply(q_tensor[i + 1][j - 1], {0.5, -0.5});
            q_divergence += matrix_multiply(q_tensor[i - 1][j - 1], {-0.5, -0.5});

            coord_d current_splay = matrix_multiply(q_tensor[i][j], q_divergence);
            splay_table[i][j] = current_splay;
        }
    }
    // Use splay vector values from edge adjacent elements
    size_t x_size = splay_table.size();
    size_t y_size = splay_table[0].size();
    for (int i = 0; i < x_size; i++) {
        splay_table[i][0] = splay_table[i][1];
        splay_table[i][y_size - 1] = splay_table[i][y_size - 2];
    }
    for (int i = 0; i < y_size; i++) {
        splay_table[0][i] = splay_table[1][i];
        splay_table[x_size - 1][i] = splay_table[x_size - 2][i];
    }
    return splay_table;
}


std::vector<std::vector<double>>
vectorArrayNorm(const std::vector<std::vector<coord_d>> &vector_array, int threads) {
    std::vector<std::vector<double>> norms(vector_array.size(), std::vector<double>(vector_array[0].size()));

    omp_set_num_threads(threads);
#pragma omp parallel for
    for (int i = 0; i < vector_array.size(); i++) {
        for (int j = 0; j < vector_array[i].size(); j++) {
            norms[i][j] = norm(vector_array[i][j]);
        }
    }
    return norms;
}

std::vector<std::vector<coord_d>> normalizeVectorArray(const std::vector<std::vector<coord_d>> &vector_array, int threads) {
    std::vector<std::vector<coord_d>> norms(vector_array.size(), std::vector<coord_d>(vector_array[0].size()));

    omp_set_num_threads(threads);
#pragma omp parallel for
    for (int i = 0; i < vector_array.size(); i++) {
        for (int j = 0; j < vector_array[i].size(); j++) {
            norms[i][j] = normalized(vector_array[i][j]);
        }
    }
    return norms;
}

std::vector<int> findNullRows(const std::vector<std::vector<uint8_t>> &array, int padding) {
    int top_empty_rows = 0;
    while (std::all_of(array[top_empty_rows].begin(),
                       array[top_empty_rows].end(),
                       [](int i) { return i == 0; }
    )) {
        top_empty_rows++;
    }

    int row_count = array.size() - 1;
    int bottom_empty_rows = 0;
    while (std::all_of(array[row_count - bottom_empty_rows].begin(),
                       array[row_count - bottom_empty_rows].end(),
                       [](int i) { return i == 0; }
    )) {
        bottom_empty_rows++;
    }
    return {top_empty_rows - padding, bottom_empty_rows - padding};
}

std::vector<int> findNullColumns(const std::vector<std::vector<uint8_t>> &array, int padding) {
    int left_empty_columns = 0;
    while (std::all_of(array.begin(),
                       array.end(),
                       [left_empty_columns](std::vector<uint8_t> vec) { return vec[left_empty_columns] == 0; }
    )) {
        left_empty_columns++;
    }

    int column_count = array[0].size();
    int right_empty_columns = 0;
    while (std::all_of(array.begin(),
                       array.end(),
                       [column_count, right_empty_columns](std::vector<uint8_t> vec) {
                           return vec[column_count - right_empty_columns - 1] == 0;
                       }
    )) {
        right_empty_columns++;
    }
    return {left_empty_columns - padding, right_empty_columns - padding};
}

std::vector<double> operator*(const std::vector<double> &self, double multiplier) {
    return multiply(self, multiplier);
}

std::vector<double> operator*(double multiplier, const std::vector<double> &self) {
    return self * multiplier;
}

std::vector<double> operator/(const std::vector<double> &self, double divisor) {
    return divide(self, divisor);
}

std::vector<double> operator/(double divisor, const std::vector<double> &self) {
    return divide(self, divisor);
}

std::vector<double> operator+(const std::vector<double> &self, const std::vector<double> &other) {
    return add(self, other);
}

std::vector<double> operator-(const std::vector<double> &self, const std::vector<double> &other) {
    return subtract(self, other);
}

std::vector<int> operator*(const std::vector<int> &self, double multiplier) {
    return multiply(self, multiplier);
}

std::vector<int> operator*(double multiplier, const std::vector<int> &self) {
    return self * multiplier;
}

std::vector<int> operator/(const std::vector<int> &self, double divisor) {
    return divide(self, divisor);
}

std::vector<int> operator/(double divisor, const std::vector<int> &self) {
    return divide(self, divisor);
}

std::vector<int> operator+(const std::vector<int> &self, const std::vector<int> &other) {
    return add(self, other);
}

std::vector<int> operator-(const std::vector<int> &self, const std::vector<int> &other) {
    return subtract(self, other);
}
//
//double norm(const coord_d &coordinate) {
//    return sqrt(coordinate.first * coordinate.first + coordinate.second * coordinate.second);
//}
//
//double norm(const coord &coordinate) {
//    return sqrt(coordinate.first * coordinate.first + coordinate.second * coordinate.second);
//}
//
//coord_d normalized(const coord_d &coordinate) {
//    double norm_value = norm(coordinate);
//    return {coordinate.first / norm_value, coordinate.second / norm_value};
//}
//
//coord_d normalized(const coord &coordinate) {
//    double norm_value = norm(coordinate);
//    return {coordinate.first / norm_value, coordinate.second / norm_value};
//}
//
//double dot(const coord_d &first, const coord_d &second) {
//    return first.first * second.first + second.first * second.second;
//}
