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
// Created by Michał Zmyślony on 21/09/2021.
//

#include "table_reading.h"

#include <iostream>
#include <fstream>
#include <sstream>


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
            if (element == "#") {
                break;
            }
            try {
                row.push_back(std::stod(element));
            }
            catch (std::invalid_argument) {
                std::cout << "Invalid element in imported double table: " << element << " in " << filename << std::endl;
                throw;
            }
        }
        table.push_back(row);
    }
    return table;
}

std::vector<std::vector<coord_d>> readFileToTableDoubleVector(const std::string &filename) {

    std::vector<std::vector<double>> table = readFileToTableDouble(filename);

    std::vector<std::vector<coord_d>> vector_table;
    for (auto &row: table) {
        std::vector<coord_d> vector_row;
        for (int i = 0; i < row.size(); i += 2) {
            vector_row.emplace_back(coord_d{row[i], row[i + 1]});
        }
        vector_table.emplace_back(vector_row);
    }

    return vector_table;
}

std::vector<std::vector<uint8_t>> tableDoubleToInt(std::vector<std::vector<double>> &double_table) {
    std::vector<std::vector<uint8_t>> int_table;
    for (auto &row: double_table) {
        std::vector<uint8_t> int_row;
        int_row.reserve(row.size());
        for (auto &element: row) {
            int_row.push_back((uint8_t) element);
        }
        int_table.push_back(int_row);
    }
    return int_table;
}


std::vector<std::vector<uint8_t>> readFileToTableInt(const std::string &filename) {
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

std::vector<int> getTableDimensions(std::string &filename) {
    std::vector<int> size = {0, 0};

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


std::vector<int> getTableDimensions(const std::vector<std::vector<uint8_t>> &table) {
    std::vector<int> size;
    size = {(int) table.size(), (int) table[0].size()};
    return size;
}

