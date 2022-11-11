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

#include "DesiredPattern.h"
#include "../auxiliary/TableReading.h"
#include "../auxiliary/SimpleMathOperations.h"
#include "../auxiliary/Perimeter.h"
#include "../auxiliary/ValarrayOperations.h"
#include <iostream>

DesiredPattern::DesiredPattern(std::string shape_filename, std::string x_vector_field_filename,
                               std::string y_vector_field_filename) :
        shape_matrix(readFileToTableInt(shape_filename)),
        x_field_preferred(readFileToTableDouble(x_vector_field_filename)),
        y_field_preferred(readFileToTableDouble(y_vector_field_filename)) {
//        dimensions(getTableDimensions(shape_filename)) {
    dimensions = getTableDimensions(shape_matrix);
    perimeter_list = findSortedPerimeters(shape_matrix, dimensions);
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
                      (1 - x_position_fraction) * (1 - y_position_fraction) * x_field_preferred[x_position + 1][y_position + 1] +
                      x_position_fraction * (1 - y_position_fraction) * x_field_preferred[x_position][y_position + 1]);

    double y_field = (x_position_fraction * y_position_fraction * y_field_preferred[x_position][y_position] +
                      (1 - x_position_fraction) * y_position_fraction * y_field_preferred[x_position + 1][y_position] +
                      (1 - x_position_fraction) * (1 - y_position_fraction) * y_field_preferred[x_position + 1][y_position + 1] +
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
    return isInShape(dtoiArray(position));
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
