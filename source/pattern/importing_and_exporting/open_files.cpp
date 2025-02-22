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
// Created by Michał Zmyślony on 24/09/2021.
//

#include "open_files.h"
#include <iostream>
#include <sstream>


std::vector<int> readConfigTable(const fs::path &config_path) {
    std::string line;
    std::ifstream file(config_path.string());
    std::vector<std::vector<std::string>> table;

    while (std::getline(file, line)) {
        std::string element;
        std::stringstream line_stream(line);
        std::vector<std::string> row;

        while (std::getline(line_stream, element, ',')) {
            row.push_back(element);
        }
        table.push_back(row);
    }

    std::vector<int> config_variables;
    config_variables.push_back(std::stoi(table[0][1]));
    config_variables.push_back(std::stoi(table[1][1]));
    config_variables.push_back(std::stoi(table[2][1]));
    return config_variables;
}

DesiredPattern openPatternFromDirectory(const fs::path &directory_path, bool is_splay_filling_enabled, int threads,
                                        const FillingMethodConfig &filling) {
    fs::path shape_path = directory_path / "shape.csv";
    fs::path theta_field_path = directory_path / "theta_field.csv";
    fs::path x_field_path = directory_path / "xField.csv";
    fs::path y_field_path = directory_path / "yField.csv";
    fs::path splay_path = directory_path / "splay.csv";

    if (!fs::exists(shape_path)) {
        throw std::runtime_error("Shape matrix does not exist in the searched directory.");
    }

    DesiredPattern pattern;
    if (fs::exists(theta_field_path)) {
        pattern = {shape_path.string(), theta_field_path.string(), is_splay_filling_enabled, threads, filling};
    } else if (fs::exists(x_field_path) && fs::exists(y_field_path)) {
        pattern = {shape_path.string(), x_field_path.string(), y_field_path.string(), is_splay_filling_enabled, threads, filling};
    } else {
        throw std::runtime_error("Neither theta nor xy field matrices are found in the searched directory.");
    }

    if (fs::exists(splay_path)) {
        pattern.setSplayVector(splay_path.string());
    } else if (is_splay_filling_enabled) {
        std::cout
                << "Splay filling is enabled but no splay file is provided. Calculating splay numerically - output "
                   "quality may be decreased" << std::endl;
    }
    pattern.updateProperties();
    return pattern;
}


FilledPattern
openFilledPatternFromDirectoryAndPattern(const fs::path &directory_path, const DesiredPattern &pattern,
                                         unsigned int seed) {
    fs::path config_path = directory_path / "config.txt";
    std::vector<int> config = readConfigTable(config_path);
    FilledPattern filled_pattern(pattern, config[0], config[1], config[2], seed);
    return filled_pattern;
}

FilledPattern openFilledPatternFromDirectoryAndPattern(const fs::path &directory_path, const DesiredPattern &pattern) {
    return openFilledPatternFromDirectoryAndPattern(directory_path, pattern, 0);
}