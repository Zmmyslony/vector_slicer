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
// Created by Michał Zmyślony on 17/09/2021.
//
#ifndef VECTOR_SLICER_DESIREDPATTERN_H
#define VECTOR_SLICER_DESIREDPATTERN_H

#include <string>
#include <vector>
#include <valarray>

class DesiredPattern {
    std::valarray<int> dimensions;
    std::vector<std::valarray<int>> perimeter_list;
    std::vector<std::vector<int>> shape_matrix;
    std::vector<std::vector<double>> x_field_preferred;
    std::vector<std::vector<double>> y_field_preferred;
public:

    const std::valarray<int> &getDimensions() const;

    const std::vector<std::valarray<int>> &getPerimeterList() const;

    const std::vector<std::vector<int>> &getShapeMatrix() const;

    const std::vector<std::vector<double>> &getXFieldPreferred() const;

    const std::vector<std::vector<double>> &getYFieldPreferred() const;

    DesiredPattern(const std::string &shape_filename, const std::string &x_field_filename,
                   const std::string &y_field_filename);

    DesiredPattern(const std::string& shape_filename, const std::string& theta_field_filename);

    DesiredPattern(std::vector<std::vector<int>> shape_field, std::vector<std::vector<double>> x_field,
                   std::vector<std::vector<double>> y_field);

    std::valarray<int> preferredDirection(const std::valarray<int> &position, int distance) const;

    std::valarray<double> preferredDirection(const std::valarray<double> &position, int distance) const;

    bool isInShape(const std::valarray<int> &position) const;

    bool isInShape(const std::valarray<double> &position) const;
};


#endif //VECTOR_SLICER_DESIREDPATTERN_H
