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
// Created by Michał Zmyślony on 23/09/2021.
//

#include <cmath>
#include <cassert>
#include <omp.h>

#include "simple_math_operations.h"
#include "valarray_operations.h"
#include "configuration_reading.h"
#include "vector_slicer_config.h"

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

std::vector<std::vector<vald>>
joinTables(const std::vector<std::vector<double>> &x_field, const std::vector<std::vector<double>> &y_field) {
    assert((x_field.size() == y_field.size()));
    std::vector<std::vector<vald>> joined_table(x_field.size(), std::vector<vald>(x_field[0].size()));

    int threads = readKeyInt(DISAGREEMENT_CONFIG, "threads");
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

std::vector<std::vector<matrix_d>> tensorWithItself(const std::vector<std::vector<std::valarray<double>>> &director) {
    std::vector<std::vector<matrix_d>> result(director.size(), std::vector<matrix_d>(director[0].size()));

    int threads = readKeyInt(DISAGREEMENT_CONFIG, "threads");
    omp_set_num_threads(threads);
#pragma omp parallel for
    for (int i = 0; i < director.size(); i++) {
        for (int j = 0; j < director[i].size(); j++) {
            result[i][j] = tensor(director[i][j], director[i][j]);
        }
    }
    return result;
}


/// Calculation of the splay from the divergence theorem where Q = n (tensor) n, and splay is Q . Div(Q).b
std::vector<std::vector<std::valarray<double>>>
splayVector(const std::vector<std::vector<double>> &x_field, const std::vector<std::vector<double>> &y_field) {
    std::vector<std::vector<vald>> director_field = joinTables(x_field, y_field);
    std::vector<std::vector<matrix_d>> q_field = tensorWithItself(director_field);

    std::vector<std::vector<vald>> splay_table(x_field.size(), std::vector<vald>(x_field[0].size()));
    splay_table.front() = {x_field[0].size(), std::valarray<double>{0, 0}};

    int threads = readKeyInt(DISAGREEMENT_CONFIG, "threads");
    omp_set_num_threads(threads);
#pragma omp parallel for
    for (int i = 1; i < director_field.size() - 1; i++) {
        for (int j = 1; j < director_field[i].size() - 1; j++) {
            std::valarray<double> q_divergence;
            q_divergence = multiply(q_field[i + 1][j], {1, 0});
            q_divergence += multiply(q_field[i][j + 1], {0, 1});
            q_divergence += multiply(q_field[i - 1][j], {-1, 0});
            q_divergence += multiply(q_field[i][j - 1], {0, -1});

            q_divergence /= 2;

            vald current_splay = multiply(q_field[i][j], q_divergence);
            splay_table[i][j] = current_splay;
        }
    }
    splay_table.back() = {x_field[0].size(), std::valarray<double>{0, 0}};
    return splay_table;
}

std::vector<std::vector<std::valarray<double>>> divergence(const std::vector<std::vector<double>> &field) {
    std::vector<std::vector<std::valarray<double>>> div(field.size(),
                                                        std::vector<std::valarray<double>>(field[0].size()));
    div.front() = {field[0].size(), std::valarray<double>{0, 0}};
    div.back() = {field[0].size(), std::valarray<double>{0, 0}};

    int threads = readKeyInt(DISAGREEMENT_CONFIG, "threads");
    omp_set_num_threads(threads);
#pragma omp parallel for
    for (int i = 1; i < field.size() - 1; i++) {
        for (int j = 1; j < field[i].size() - 1; j++) {
            div[i][j] = {field[i + 1][j] - field[i - 1][j],
                         field[i][j + 1] - field[i][j - 1]};
        }
    }
    return div;
}

std::vector<std::vector<double>>
normalizeVectorArray(const std::vector<std::vector<std::valarray<double>>> &vector_array) {
    std::vector<std::vector<double>> norms(vector_array.size(), std::vector<double>(vector_array[0].size()));

    int threads = readKeyInt(DISAGREEMENT_CONFIG, "threads");
    omp_set_num_threads(threads);
#pragma omp parallel for
    for (int i = 0; i < vector_array.size(); i++) {
        for (int j = 0; j < vector_array[i].size(); j++) {
            norms[i][j] = norm(vector_array[i][j]);
        }
    }
    return norms;
}