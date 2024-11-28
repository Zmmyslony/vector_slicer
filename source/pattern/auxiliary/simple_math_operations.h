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
#include <stdexcept>
#include <algorithm>

using coord = std::pair<uint16_t, uint16_t>;
using coord_d = std::pair<double, double>;

using vecd = std::vector<double>;


int roundUp(double);

int sgn(double number);

double decimalPart(double number);

std::vector<std::vector<std::vector<double>>>
splayVector(const std::vector<std::vector<double>> &x_field, const std::vector<std::vector<double>> &y_field,
            int threads);

std::vector<std::vector<double>>
vectorArrayNorm(const std::vector<std::vector<std::vector<double>>> &vector_array, int threads);

std::vector<std::vector<vecd>> normalizeVectorArray(const std::vector<std::vector<vecd>> &vector_array, int threads);

std::vector<int> findNullRows(const std::vector<std::vector<int>> &array, int padding);

std::vector<int> findNullColumns(const std::vector<std::vector<int>> &array, int padding);

template<typename T>
std::vector<std::vector<T>> zero_like(std::vector<std::vector<T>> obj) {
    for (auto &el: obj) {
        for (auto &sub_el: el) {
            sub_el = 0;
        }
    }
    return obj;
}

template<typename T>
std::vector<std::valarray<T>> zero_like(std::vector<std::valarray<T>> obj) {
    for (auto &el: obj) {
        for (auto &sub_el: el) {
            sub_el = 0;
        }
    }
    return obj;
}

template<typename T>
std::vector<T> zero_like(std::vector<T> obj) {
    for (auto &el: obj) {
        el = 0;
    }
    return obj;
}

template<typename T>
std::valarray<T> zero_like(std::valarray<T> obj) {
    for (auto &el: obj) {
        el = 0;
    }
    return obj;
}

template<typename T>
T zero_like(T obj) {
    return 0;
}

template<typename T>
std::vector<T> adjust_rows(std::vector<T> &array, const std::vector<int> &rows_to_remove) {
    if (rows_to_remove[0] > 0) {
        array.erase(array.begin(), array.begin() + rows_to_remove[0]);
    } else if (rows_to_remove[0] < 0) {
        auto it = array.begin();
        T zeros = zero_like(array[0]);
        array.insert(it, -rows_to_remove[0], zeros);
    }
    if (rows_to_remove[1] > 0) {
        array.erase(array.end() - rows_to_remove[1], array.end());
    } else if (rows_to_remove[1] < 0) {
        auto it = array.end();
        T zeros = zero_like(array[0]);
        array.insert(it, -rows_to_remove[1], zeros);
    }
    return array;
}

template<typename T>
std::vector<std::vector<T>>
adjust_columns(std::vector<std::vector<T>> &array, const std::vector<int> &columns_to_remove) {
    for (auto &row: array) {
        adjust_rows(row, columns_to_remove);
    }
    return array;
}

template<typename T>
void adjustRowsAndColumns(std::vector<std::vector<T>> &array, const std::vector<int> &rows_to_remove,
                          const std::vector<int> &columns_to_remove) {
    adjust_rows(array, rows_to_remove);
    adjust_columns(array, columns_to_remove);

}

/// Minimum value of 2D vector array
template<typename T>
T min_array(const std::vector<std::vector<T>> &array) {
    std::vector<T> arr_min(array.size());
    for (auto &vec: array) {
        T element = *std::min_element(vec.begin(), vec.end());
        arr_min.emplace_back(element);
    }
    return *std::min_element(arr_min.begin(), arr_min.end());
};

/// Maximum value of 2D vector array
template<typename T>
T max_array(const std::vector<std::vector<T>> &array) {
    std::vector<T> arr_min(array.size());
    for (auto &vec: array) {
        T element = *std::max_element(vec.begin(), vec.end());
        arr_min.emplace_back(element);
    }
    return *std::max_element(arr_min.begin(), arr_min.end());
};

template<typename T>
std::vector<T> add(std::vector<T> obj_one, const std::vector<T> &obj_two) {
    if (obj_one.size() != obj_two.size()) {
        throw std::runtime_error("Cannot add two vectors of uneven sizes.");
    }
    for (int i = 0; i < obj_one.size(); i++) {
        obj_one[i] += obj_two[i];
    }
    return obj_one;
}

template<typename T>
std::vector<T> subtract(std::vector<T> obj_one, const std::vector<T> &obj_two) {
    if (obj_one.size() != obj_two.size()) {
        throw std::runtime_error("Cannot add two vectors of uneven sizes.");
    }
    for (int i = 0; i < obj_one.size(); i++) {
        obj_one[i] -= obj_two[i];
    }
    return obj_one;
}

template<typename T>
std::vector<T> multiply(std::vector<T> obj_one, double multiplier) {
    for (int i = 0; i < obj_one.size(); i++) {
        obj_one[i] *= multiplier;
    }
    return obj_one;
}

template<typename T>
std::vector<T> divide(std::vector<T> obj_one, double divisor) {
    for (int i = 0; i < obj_one.size(); i++) {
        obj_one[i] /= divisor;
    }
    return obj_one;
}

template<typename T>
std::vector<T> abs(std::vector<T> obj_one) {
    for (int i = 0; i < obj_one.size(); i++) {
        obj_one[i] = std::abs(obj_one[i]);
    }
    return obj_one;
}

std::vector<double> operator*(const std::vector<double> &self, double multiplier);
std::vector<double> operator*(double multiplier, const std::vector<double> &self);
std::vector<double> operator/(const std::vector<double> &self, double divisor);

std::vector<double> operator/(double divisor, const std::vector<double> &self);

std::vector<double> operator+(const std::vector<double> &self, const std::vector<double> &other);

std::vector<double> operator-(const std::vector<double> &self, const std::vector<double> &other);

std::vector<int> operator*(const std::vector<int> &self, double multiplier);

std::vector<int> operator*(double multiplier, const std::vector<int> &self);

std::vector<int> operator/(const std::vector<int> &self, double divisor);

std::vector<int> operator/(double divisor, const std::vector<int> &self);

std::vector<int> operator+(const std::vector<int> &self, const std::vector<int> &other);

std::vector<int> operator-(const std::vector<int> &self, const std::vector<int> &other);

double norm(const coord_d &coordinate);

double norm(const coord &coordinate);

coord_d normalized(const coord_d &coordinate);

coord_d normalized(const coord &coordinate);

double dot(const coord_d &first, const coord_d &second);

coord_d operator+(const coord_d &first, const coord_d &second);

coord_d operator-(const coord_d &first, const coord_d &second);

coord_d operator*(const coord_d &first, double multiplier);

coord_d operator/(const coord_d &first, double divisor);


#endif //VECTOR_SLICER_SIMPLE_MATH_OPERATIONS_H
