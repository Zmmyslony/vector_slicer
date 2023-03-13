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
// Created by Michał Zmyślony on 13/10/2021.
//

#include "Exporting.h"
#include <fstream>
#include <sstream>
#include <ctime>
#include "vector_slicer_config.h"


std::string readRowToString(const std::vector<int> &row) {
    std::string row_string;
    for (auto &element: row) {
        row_string += std::to_string(element);
        row_string += ",";
    }
    row_string.pop_back();
    row_string += "\n";
    return row_string;
}


std::string readRowToString(const std::vector<double> &row) {
    std::string row_string;
    for (auto &element: row) {
        row_string += std::to_string(element);
        row_string += ",";
    }
    row_string.pop_back();
    row_string += "\n";
    return row_string;
}


void exportVectorTableToFile(const std::vector<std::vector<int>> &table, fs::path &filename) {
    std::ofstream file(filename.string());
    if (file.is_open()) {
        for (auto &row: table) {
            file << readRowToString(row);
        }
        file.close();
    }
}


void exportVectorTableToFile(const std::string &header, const std::vector<std::vector<int>> &table_first,
                             const std::vector<std::vector<int>> &table_second, fs::path &filename) {
    std::ofstream file(filename.string());

    if (file.is_open()) {
        file << header;
        for (int i = 0; i < table_first.size(); i++) {
            std::vector<int> row;
            for (int j = 0; j < table_first[i].size(); j++) {
                row.emplace_back(table_first[i][j]);
                row.emplace_back(table_second[i][j]);
            }
            file << readRowToString(row);
        }
        file.close();
    }
}


void exportVectorTableToFile(const std::vector<std::vector<double>> &table, fs::path &filename) {
    std::ofstream file(filename.string());
    if (file.is_open()) {
        for (auto &row: table) {
            file << readRowToString(row);
        }
    }
    file.close();
}


std::vector<std::vector<int>> indexTable(const std::vector<std::vector<std::valarray<int>>> &grid_of_coordinates,
                                         int index) {
    std::vector<std::vector<int>> table;
    table.reserve(grid_of_coordinates.size());
    for (auto &row: grid_of_coordinates) {
        std::vector<int> new_row;
        new_row.reserve(row.size());
        for (auto &element: row) {
            new_row.push_back(element[index]);
        }
        table.push_back(new_row);
    }
    return table;
}

std::string generateHeader(const std::string &pattern_name, double print_diameter) {
    std::string header;
    time_t ttime = time(nullptr);
    char time[26];

#if defined(_WIN32) || defined(_WIN64)
    ctime_s(time, sizeof time, &ttime);
#endif
#ifdef __linux__
    ctime_r(&ttime, time);
#endif

    header += "# Generated using Vector Slicer " + std::string(SLICER_VER) + " on " + time;
    header += "# Michal Zmyslony, University of Cambridge, mlz22@cam.ac.uk\n";
    header += "# Source directory: " + pattern_name + "\n";
    header += "# Print diameter: " + std::to_string(print_diameter);
    return header;
}


void
exportPathSequence(const std::vector<std::vector<std::valarray<int>>> &grid_of_coordinates, const fs::path &path,
                   const std::string &suffix, double print_diameter) {
    std::vector<std::vector<int>> x_table = indexTable(grid_of_coordinates, 0);
    std::vector<std::vector<int>> y_table = indexTable(grid_of_coordinates, 1);

    fs::path paths_filename = path / (suffix + ".csv");
    exportVectorTableToFile(generateHeader(suffix, print_diameter), x_table, y_table, paths_filename);
}


std::vector<std::vector<int>> importTableInt(const fs::path &filename) {
    std::vector<std::vector<int>> table;
    std::string line;
    std::fstream file(filename.string());

    while (std::getline(file, line)) {
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        std::string element;
        std::stringstream line_stream(line);
        std::vector<int> row;

        while (std::getline(line_stream, element, ',')) {
            row.push_back(stoi(element));
        }
        table.push_back(row);
    }
    return table;
}

std::vector<std::vector<std::valarray<int>>> mergeTwoTables(const std::vector<std::vector<int>> &x_table,
                                                            const std::vector<std::vector<int>> &y_table) {
    std::vector<std::vector<std::valarray<int>>> merged_table;
    for (int i = 0; i < x_table.size(); i++) {
        std::vector<std::valarray<int>> merged_row;
        for (int j = 0; j < x_table[i].size(); j++) {
            std::valarray<int> merged_element = {x_table[i][j], y_table[i][j]};
            merged_row.push_back(merged_element);
        }
        merged_table.push_back(merged_row);
    }
    return merged_table;
}


std::vector<std::vector<std::valarray<int>>> read3DVectorFromFile(const fs::path &path, const std::string &suffix) {
    fs::path x_filename = path / ("x_" + suffix + ".csv");
    fs::path y_filename = path / ("y_" + suffix + ".csv");

    std::vector<std::vector<int>> x_table = importTableInt(x_filename);
    std::vector<std::vector<int>> y_table = importTableInt(y_filename);

    std::vector<std::vector<std::valarray<int>>> merged_tables = mergeTwoTables(x_table, y_table);
    return merged_tables;
}

