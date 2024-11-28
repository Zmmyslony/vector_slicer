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
// Created by Michał Zmyślony on 13/10/2021.
//

#ifndef VECTOR_SLICER_EXPORTING_H
#define VECTOR_SLICER_EXPORTING_H

#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/dll.hpp>
#include "../simulation/simulation.h"
#include "../coord.h"

namespace fs = boost::filesystem;
using veci = std::vector<int>;
using vecd = std::vector<double>;


//void exportVectorTableToFile(const std::vector<std::vector<int>> &table, const fs::path &path);
//
//void exportVectorTableToFile(const std::vector<std::vector<double>> &table, const fs::path &filename);

void exportVectorTableToFile(const std::string &header, const std::vector<std::vector<int>> &table_first,
                             const std::vector<std::vector<int>> &table_second, fs::path &filename);

void exportPathSequence(const std::vector<std::vector<std::valarray<int>>> &grid_of_coordinates, const fs::path &path,
                        const std::string &suffix, double print_diameter);

void exportPathSequence(const std::vector<std::vector<std::vector<std::vector<int>>>> &grids_of_paths, const fs::path path,
                        const std::string &suffix, double print_diameter, const Simulation &simulation);

std::vector<std::vector<std::vector<int>>> read3DVectorFromFile(const fs::path &path, const std::string &suffix);

void exportVector(const std::vector<veci> &vector, const std::string &filename);

void exportVector(const std::vector<vecd> &vector, const std::string &filename);

void printVector(const std::vector<vecd> &vector);

void printVector(const std::vector<veci> &vector);

void exportOverlap(const std::vector<std::vector<std::vector<double>>> &overlap_stack, const fs::path &path,
                   const std::string &suffix, double print_diameter, const Simulation &simulation);

void exportCoordVector(const coord_vector &vector, const fs::path &filename);

//void exportRowToFile(const std::vector<unsigned int> &row, const fs::path &path);

template<typename T>
std::string readRowToString(const std::vector<T> &row) {
    std::string row_string;
    for (auto &element: row) {
        row_string += std::to_string(element);
        row_string += ",";
    }
    row_string.pop_back();
    row_string += "\n";
    return row_string;
}

template<typename T>
void exportRowToFile(const std::vector<T> &row, const fs::path &path) {
    std::ofstream file(path.string());
    if (file.is_open()) {
        file << readRowToString(row);
        file.close();
    }
}

template<typename T>
void exportVectorTableToFile(const std::vector<std::vector<T>> &table, const fs::path &path) {
    std::ofstream file(path.string());
    if (file.is_open()) {
        for (auto &row: table) {
            file << readRowToString(row);
        }
        file.close();
    }
}
#endif //VECTOR_SLICER_EXPORTING_H
