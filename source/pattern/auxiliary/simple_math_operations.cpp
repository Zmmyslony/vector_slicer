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

std::vector<std::vector<std::valarray<double>>>
splayVector(const std::vector<std::vector<double>> &x_field, const std::vector<std::vector<double>> &y_field) {
    std::vector<std::vector<std::valarray<double>>> splay_table;
    splay_table.emplace_back(x_field[0].size(), std::valarray<double>{0, 0});
    assert((x_field.size() == y_field.size()));
    for (int i = 1; i < x_field.size() - 1; i++) {
        assert((x_field[i].size() == y_field[i].size()));
        std::vector<std::valarray<double>> splay_row = {{0, 0}};
        for (int j = 1; j < x_field[i].size() - 1; j++) {
            std::valarray<double> current_splay = {x_field[i + 1][j] - x_field[i - 1][j], y_field[i][j + 1] - x_field[i][j - 1]};
            splay_row.emplace_back(current_splay);
        }
        splay_row.emplace_back(0);
        splay_table.emplace_back(splay_row);
    }
    splay_table.emplace_back(x_field[0].size(), std::valarray<double>{0, 0});
    return splay_table;
}

std::vector<std::vector<std::valarray<double>>> gradient(const std::vector<std::vector<double>> &field) {
    std::vector<std::vector<std::valarray<double>>> div(field.size(),
                                                        std::vector<std::valarray<double>>(field[0].size()));
    for (int i = 0; i < field.size(); i++) {
        div[i][0] = {0, 0};
        div[i][field[i].size() - 1] = {0, 0};
    }
    for (int j = 0; j < field[0].size(); j++) {
        div[0][j] = {0, 0};
        div[field.size() - 1][j] = {0, 0};
    }

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
    for (int i = 0; i < vector_array.size(); i++) {
        for (int j = 0; j < vector_array[i].size(); j++) {
            norms[i][j] = norm(vector_array[i][j]);
        }
    }
    return norms;
}