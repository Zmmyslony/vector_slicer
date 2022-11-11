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
        std::stringstream lineStream(line);
        std::vector<double> row;

        while (std::getline(lineStream, element, ',')) {
            row.push_back(std::stod(element));
        }
        table.push_back(row);
    }
    return table;
}

std::vector<std::vector<int>> tableDoubleToInt(std::vector<std::vector<double>> &doubleTable) {
    std::vector<std::vector<int>> intTable;
    for (auto &row: doubleTable) {
        std::vector<int> intRow;
        intRow.reserve(row.size());
        for (auto &element: row) {
            intRow.push_back((int) element);
        }
        intTable.push_back(intRow);
    }
    return intTable;
}


std::vector<std::vector<int>> readFileToTableInt(const std::string &filename) {
    std::vector<std::vector<double>> doubleTable = readFileToTableDouble(filename);
    return (tableDoubleToInt(doubleTable));
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
        std::stringstream lineStream(line);
        if (size[0] == 0) {
            while (std::getline(lineStream, element, ',')) {
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

