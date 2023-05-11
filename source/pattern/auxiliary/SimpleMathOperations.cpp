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

#include "SimpleMathOperations.h"
#include <cmath>
#include <cassert>

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

std::vector<std::vector<double>>
splay(const std::vector<std::vector<double>> &x_field, const std::vector<std::vector<double>> &y_field) {
    std::vector<std::vector<double>> splay_table;
    splay_table.emplace_back(x_field[0].size(), 0);
    assert((x_field.size() == y_field.size()));
    for (int i = 1; i < x_field.size() - 1; i++) {
        assert((x_field[i].size() == y_field[i].size()));
        std::vector<double> splay_row = {0};
        for (int j = 1; j < x_field[i].size() - 1; j++) {
            double current_splay = pow(x_field[i + 1][j] - x_field[i - 1][j], 2) +
                                   pow(y_field[i][j + 1] - x_field[i][j - 1], 2);
            splay_row.emplace_back(current_splay);
        }
        splay_row.emplace_back(0);
        splay_table.emplace_back(splay_row);
    }
    splay_table.emplace_back(x_field[0].size(), 0);
    return splay_table;
}