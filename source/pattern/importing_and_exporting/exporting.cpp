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

#include "exporting.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include "vector_slicer_config.h"
#include "../simulation/configuration_reading.h"
#include "../simulation/simulation.h"


std::stringstream convertVectorTableToStream(const std::vector<std::vector<int>> &table_first,
                                             const std::vector<std::vector<int>> &table_second) {
    std::stringstream stream;
    for (int i = 0; i < table_first.size(); i++) {
        std::vector<int> row;
        for (int j = 0; j < table_first[i].size(); j++) {
            row.emplace_back(table_first[i][j]);
            row.emplace_back(table_second[i][j]);
        }
        stream << readRowToString(row);
    }
    return stream;
}

std::stringstream convertVectorTableToStream(const std::vector<std::vector<double>> &table) {
    std::stringstream stream;
    for (const auto &row: table) {
        stream << readRowToString(row);
    }
    return stream;
}

void exportHeaderToFile(const std::string &header, const fs::path &filename) {
    std::ofstream file(filename.string());

    if (file.is_open()) {
        file << header;
    } else {
        throw std::runtime_error("File " + filename.string() + " failed to create.");
    }
}


void appendVectorTableToFile(const std::vector<std::vector<int>> &table_first,
                             const std::vector<std::vector<int>> &table_second, const fs::path &filename) {
    std::ofstream file(filename.string(), std::ios_base::app);

    if (file.is_open()) {
        file << std::endl;
        file << "# Start of pattern" << std::endl;
        file << convertVectorTableToStream(table_first, table_second).rdbuf();
        file << "# End of pattern" << std::endl;
    }
}

void appendVectorTableToFile(const std::vector<std::vector<double>> &table, const fs::path &filename) {
    std::ofstream file(filename.string(), std::ios_base::app);

    if (file.is_open()) {
        file << std::endl;
        file << "# Start of pattern" << std::endl;
        file << convertVectorTableToStream(table).rdbuf();
        file << "# End of pattern" << std::endl;
    }
}


void exportVectorTableToFile(const std::string &header, const std::vector<std::vector<int>> &table_first,
                             const std::vector<std::vector<int>> &table_second, const fs::path &filename) {
    exportHeaderToFile(header, filename);
    appendVectorTableToFile(table_first, table_second, filename);
}


void exportVectorTableToFile(const std::vector<std::vector<double>> &table, const fs::path &filename) {
    std::ofstream file(filename.string());
    if (file.is_open()) {
        for (auto &row: table) {
            file << readRowToString(row);
        }
    }
    file.close();
}


std::vector<std::vector<int>> indexTable(const std::vector<std::vector<coord>> &grid_of_coordinates,
                                         int index) {
    std::vector<std::vector<int>> table;
    table.reserve(grid_of_coordinates.size());
    for (auto &row: grid_of_coordinates) {
        std::vector<int> new_row;
        new_row.reserve(row.size());
        for (auto &element: row) {
            if (index == 0) {
                new_row.push_back(element.first);
            } else if (index == 1) {
                new_row.push_back(element.second);
            }
        }
        table.push_back(new_row);
    }
    return table;
}

std::string generateHeader(const std::string &pattern_name, double print_diameter, const Simulation &simulation) {
    time_t ttime = time(nullptr);
    char time[26];

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    ctime_s(time, sizeof time, &ttime);
#elif __linux__
    ctime_r(&ttime, time);
#elif __APPLE__
    ctime_r(&ttime, time);
#else
    error "Unknown compiler"
#endif
    ;
    std::stringstream header_s;
    header_s << std::setprecision(2);
    header_s << "# Generated using Vector Slicer " << std::endl
             << "# Author: Michal Zmyslony, University of Cambridge, mlz22@cam.ac.uk" << std::endl
             << "# Version: " << SLICER_VER << std::endl
             << "# Creation date: " << time << std::endl
             << "# Source directory: " << pattern_name << std::endl
             << "# Print diameter: " << print_diameter << std::endl;
    header_s << std::endl;

    header_s << "# Disagreement configuration:" << std::endl
             << "# \t p  ** " << simulation.getPathsPower() << " * ("
             << simulation.getEmptySpotWeight() << " * h ** " << simulation.getEmptySpotPower() << " + "
             << simulation.getOverlapWeight() << " * o ** " << simulation.getOverlapPower() << " + "
             << simulation.getDirectorWeight() << " * d ** " << simulation.getDirectorPower() << std::endl
             << "# \twhere p - number of paths, h - hole density, o - overlap density, d - director disagreement density";
    header_s << std::endl;

    int iteration_limit = simulation.getTotalIterations();
    header_s << "# Bayesian configuration:" << std::endl;
    if (iteration_limit <= 0) {
        header_s << "# \tUnlimited optimisation iteration cap";
    } else {
        header_s << "# \tOptimisation iteration cap: " << iteration_limit;
    }
    header_s << ", relearning period: "
             << simulation.getImprovementIterations() << ", noise: " << simulation.getNoise() << std::endl;

    header_s << "# Filling configuration:" << std::endl;

    header_s << std::endl;
    return header_s.str();
}


void
exportPathSequence(const std::vector<std::vector<std::vector<coord>>> &grids_of_paths, const fs::path path,
                   const std::string &suffix, double print_diameter, const Simulation &simulation) {
    std::string header = generateHeader(suffix, print_diameter, simulation);
    exportHeaderToFile(header, path);

    for (auto &grid: grids_of_paths) {
        std::vector<std::vector<int>> x_table = indexTable(grid, 0);
        std::vector<std::vector<int>> y_table = indexTable(grid, 1);

        appendVectorTableToFile(x_table, y_table, path);
    }
}

void exportOverlap(const std::vector<std::vector<std::vector<double>>> &overlap_stack, const fs::path &path,
                   const std::string &suffix, double print_diameter, const Simulation &simulation) {
    std::string header = generateHeader(suffix, print_diameter, simulation);
    exportHeaderToFile(header, path);

    for (auto &overlaps: overlap_stack) {
        appendVectorTableToFile(overlaps, path);
    }
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

std::vector<std::vector<std::vector<int>>> mergeTwoTables(const std::vector<std::vector<int>> &x_table,
                                                          const std::vector<std::vector<int>> &y_table) {
    std::vector<std::vector<std::vector<int>>> merged_table;
    for (int i = 0; i < x_table.size(); i++) {
        std::vector<std::vector<int>> merged_row;
        for (int j = 0; j < x_table[i].size(); j++) {
            std::vector<int> merged_element = {x_table[i][j], y_table[i][j]};
            merged_row.push_back(merged_element);
        }
        merged_table.push_back(merged_row);
    }
    return merged_table;
}


std::vector<std::vector<std::vector<int>>> read3DVectorFromFile(const fs::path &path, const std::string &suffix) {
    fs::path x_filename = path / ("x_" + suffix + ".csv");
    fs::path y_filename = path / ("y_" + suffix + ".csv");

    std::vector<std::vector<int>> x_table = importTableInt(x_filename);
    std::vector<std::vector<int>> y_table = importTableInt(y_filename);

    std::vector<std::vector<std::vector<int>>> merged_tables = mergeTwoTables(x_table, y_table);
    return merged_tables;
}

void exportVector(const std::vector<veci> &vector, const std::string &filename) {
    std::ofstream file(filename, std::ios_base::app);

    if (file.is_open()) {
        for (auto &point: vector) {
            file << point[0] << "," << point[1] << std::endl;
        }
    }
}

void exportVector(const std::vector<vecd> &vector, const std::string &filename) {
    std::ofstream file(filename, std::ios_base::app);

    if (file.is_open()) {
        for (auto &point: vector) {
            file << point[0] << "," << point[1] << std::endl;
        }
    }
}

void printVector(const std::vector<vecd> &vector) {
    for (auto &point: vector) {
        std::cout << point[0] << "," << point[1] << std::endl;
    }
}

void printVector(const std::vector<veci> &vector) {
    for (auto &point: vector) {
        std::cout << point[0] << "," << point[1] << std::endl;
    }
}

void exportCoordVector(const coord_vector &vector, const fs::path &filename) {
    std::ofstream file(filename.string());
    if (file.is_open()) {
        for (auto &coord: vector) {
            file << coord.first << "," << coord.second << std::endl;
        }
    }
    file.close();
}
