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
        std::stringstream line_stream(line);
        std::vector<std::string> row;

        while (std::getline(line_stream, element, ',')) {
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