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
// You should have received a copy of the GNU General Public License along with Vector Slicer. If not, see <https://www.gnu.org/licenses/>.

//
// Created by Michał Zmyślony on 17/09/2021.
//

#include "DesiredPattern.h"

#include <utility>
#include "../importing_and_exporting/TableReading.h"
#include "../auxiliary/SimpleMathOperations.h"
#include "../auxiliary/Perimeter.h"
#include "../auxiliary/ValarrayOperations.h"


DesiredPattern::DesiredPattern(std::vector<std::vector<int>> shape_field, std::vector<std::vector<double>> x_field,
                               std::vector<std::vector<double>> y_field) :
        shape_matrix(std::move(shape_field)),
        x_field_preferred(std::move(x_field)),
        y_field_preferred(std::move(y_field)),
        dimensions(getTableDimensions(shape_field)) {

    perimeter_list = findSortedPerimeters(shape_matrix, dimensions);
    splay_array = splay(x_field_preferred, y_field_preferred);
}

DesiredPattern::DesiredPattern(const std::string &shape_filename, const std::string &x_field_filename,
                               const std::string &y_field_filename) :
        DesiredPattern(readFileToTableInt(shape_filename), readFileToTableDouble(x_field_filename),
                       readFileToTableDouble(y_field_filename)) {}


DesiredPattern::DesiredPattern(const std::string& shape_filename, const std::string& theta_field_filename) {
    std::vector<std::vector<double>> theta_field = readFileToTableDouble(theta_field_filename);
    std::vector<std::vector<double>> x_field;
    std::vector<std::vector<double>> y_field;
    for (const auto& theta_row : theta_field) {
        std::vector<double> x_row;
        std::vector<double> y_row;
        for (const auto &theta : theta_row) {
            x_row.push_back(cos(theta));
            y_row.push_back(sin(theta));
        }
        x_field.push_back(x_row);
        y_field.push_back(y_row);
    }
    *this = DesiredPattern(readFileToTableInt(shape_filename), x_field, y_field);
}


std::valarray<int> DesiredPattern::preferredDirection(const std::valarray<int> &position, int distance) const {
    return std::valarray<int>{roundUp(distance * x_field_preferred[position[0]][position[1]]),
                              roundUp(distance * y_field_preferred[position[0]][position[1]])};
}


std::valarray<double> DesiredPattern::preferredDirection(const std::valarray<double> &position, int distance) const {
    double x_position_fraction = decimalPart(position[0]);
    double y_position_fraction = decimalPart(position[1]);
    unsigned int x_position = (int) floor(position[0]);
    unsigned int y_position = (int) floor(position[1]);
    double x_field = (x_position_fraction * y_position_fraction * x_field_preferred[x_position][y_position] +
                      (1 - x_position_fraction) * y_position_fraction * x_field_preferred[x_position + 1][y_position] +
                      (1 - x_position_fraction) * (1 - y_position_fraction) *
                      x_field_preferred[x_position + 1][y_position + 1] +
                      x_position_fraction * (1 - y_position_fraction) * x_field_preferred[x_position][y_position + 1]);

    double y_field = (x_position_fraction * y_position_fraction * y_field_preferred[x_position][y_position] +
                      (1 - x_position_fraction) * y_position_fraction * y_field_preferred[x_position + 1][y_position] +
                      (1 - x_position_fraction) * (1 - y_position_fraction) *
                      y_field_preferred[x_position + 1][y_position + 1] +
                      x_position_fraction * (1 - y_position_fraction) * y_field_preferred[x_position][y_position + 1]);

    if (x_field == 0 && y_field == 0) {
        return std::valarray<double>{x_field_preferred[x_position - 1][y_position],
                                     y_field_preferred[x_position - 1][y_position]};
    }
    std::valarray<double> new_step = {x_field, y_field};
    new_step = distance * normalize(new_step);
    return new_step;
}


bool DesiredPattern::isInShape(const std::valarray<int> &position) const {
    return shape_matrix[position[0]][position[1]];
}


bool DesiredPattern::isInShape(const std::valarray<double> &position) const {
    return isInShape(dtoi(position));
}

const std::valarray<int> &DesiredPattern::getDimensions() const {
    return dimensions;
}

const std::vector<std::valarray<int>> &DesiredPattern::getPerimeterList() const {
    return perimeter_list;
}

const std::vector<std::vector<int>> &DesiredPattern::getShapeMatrix() const {
    return shape_matrix;
}

const std::vector<std::vector<double>> &DesiredPattern::getXFieldPreferred() const {
    return x_field_preferred;
}

const std::vector<std::vector<double>> &DesiredPattern::getYFieldPreferred() const {
    return y_field_preferred;
}
