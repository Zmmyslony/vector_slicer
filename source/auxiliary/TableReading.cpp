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
// Created by Michał Zmyślony on 21/09/2021.
//

#include "TableReading.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>

std::vector<std::vector<double>> readFileToTableDouble(const std::string &filename) {
    std::string line;
    std::ifstream file(filename);
    if (file.fail()) {
        std::cout << "File \"" << filename << "\" does not exist!" << std::endl;
    }
    std::vector<std::vector<double>> table;

    while (std::getline(file, line)) {
        std::string element;
        std::stringstream line_stream(line);
        std::vector<double> row;

        while (std::getline(line_stream, element, ',')) {
            row.push_back(std::stod(element));
        }
        table.push_back(row);
    }
    return table;
}

std::vector<std::vector<int>> tableDoubleToInt(std::vector<std::vector<double>> &double_table) {
    std::vector<std::vector<int>> int_table;
    for (auto &row: double_table) {
        std::vector<int> int_row;
        int_row.reserve(row.size());
        for (auto &element: row) {
            int_row.push_back((int) element);
        }
        int_table.push_back(int_row);
    }
    return int_table;
}


std::vector<std::vector<int>> readFileToTableInt(const std::string &filename) {
    std::vector<std::vector<double>> double_table = readFileToTableDouble(filename);
    return (tableDoubleToInt(double_table));
}

//std::vector<std::vector<bool>> readFileToTableBool(std::string &filename) {
//    std::vector<std::vector<double>> doubleTable = readFileToTableDouble(filename);
//    return(tableDoubleToBool(doubleTable));
//}

//std::vector<std::vector<unsigned int>> readFileToTableUint(std::string &filename) {
//    std::vector<std::vector<double>> doubleTable = readFileToTableDouble(filename);
//    return(tableDoubleToUint(doubleTable));
//}

std::valarray<int> getTableDimensions(std::string &filename) {
    std::valarray<int> size = {0, 0};

    std::string line;
    std::ifstream file(filename);

    while (std::getline(file, line)) {
        std::string element;
        std::stringstream line_stream(line);
        if (size[0] == 0) {
            while (std::getline(line_stream, element, ',')) {
                size[1]++;
            }
        }
        size[0]++;
    }
    return size;
}


std::valarray<int> getTableDimensions(const std::vector<std::vector<int>> &table) {
    std::valarray<int> size;
    size = {(int)table.size(), (int)table[0].size()};
    return size;
}

