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
// Created by Michał Zmyślony on 24/09/2021.
//

#include "OpenFiles.h"
#include <iostream>
#include <fstream>
#include <sstream>

std::vector<int> readConfigTable(const std::string &configPath) {
    std::string line;
    std::ifstream file(configPath);
    std::vector<std::vector<std::string>> table;

    while (std::getline(file, line)) {
        std::string element;
        std::stringstream lineStream(line);
        std::vector<std::string> row;

        while (std::getline(lineStream, element, ',')) {
            row.push_back(element);
        }
        table.push_back(row);
    }

    std::vector<int> configVariables;
    configVariables.push_back(std::stoi(table[0][1]));
    configVariables.push_back(std::stoi(table[1][1]));
    configVariables.push_back(std::stoi(table[2][1]));
    return configVariables;
}

DesiredPattern openPatternFromDirectory(const std::string &directoryPath) {
    std::string shapePath = directoryPath + "\\shape.csv";
    std::string xFieldPath = directoryPath + "\\xField.csv";
    std::string yFieldPath = directoryPath + "\\yField.csv";

    DesiredPattern desiredPattern(shapePath, xFieldPath, yFieldPath);
    return desiredPattern;
}


FilledPattern openFilledPatternFromDirectory(const std::string &directoryPath, unsigned int seed) {
    DesiredPattern desiredPattern = openPatternFromDirectory(directoryPath);
    std::string configPath = directoryPath + "\\config.txt";
    std::vector<int> config = readConfigTable(configPath);
    FilledPattern pattern(desiredPattern, config[0], config[1], config[2], seed);
    return pattern;
}


FilledPattern openFilledPatternFromDirectory(const std::string &directoryPath) {
    return openFilledPatternFromDirectory(directoryPath, 0);
}

FilledPattern
openFilledPatternFromDirectoryAndPattern(const std::string &directoryPath, const DesiredPattern &pattern, unsigned int seed) {
    std::string configPath = directoryPath + "\\config.txt";
    std::vector<int> config = readConfigTable(configPath);
    FilledPattern filledPattern(pattern, config[0], config[1], config[2], seed);
    return filledPattern;
}

FilledPattern openFilledPatternFromDirectoryAndPattern(const std::string &directoryPath, const DesiredPattern &pattern) {
    return openFilledPatternFromDirectoryAndPattern(directoryPath, pattern, 0);
}