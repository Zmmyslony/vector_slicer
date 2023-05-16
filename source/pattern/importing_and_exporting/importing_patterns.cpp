// Copyright (c) 2022-2023, Michał Zmyślony, mlz22@cam.ac.uk.
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
// Created by Michał Zmyślony on 07/12/2022.
//

#include <iostream>

#include "vector_slicer_config.h"
#include "importing_patterns.h"
#include "../auxiliary/configuration_reading.h"

void convertSlashesOperatingSystem(std::string &string) {
#ifdef _WIN32
    std::replace(string.begin(), string.end(), '/', '\\');
#endif

#ifdef linux
    std::replace(string.begin(), string.end(), '\\', '/');
#endif
}

std::vector<fs::path> getPatterns(const fs::path& list_of_patterns_path) {
    std::vector<fs::path> patterns;
    std::fstream file(list_of_patterns_path.string());
    std::string line;

    fs::path patterns_directory = PATTERNS_SOURCE_DIRECTORY;

    while (std::getline(file, line)) {
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        convertSlashesOperatingSystem(line);

        fs::path test_path = patterns_directory / line;
        if (exists(test_path) && !line.empty() > 0 && !isspace(line[0]) ) {
            patterns.emplace_back(test_path);

        }
    }
    return patterns;
}