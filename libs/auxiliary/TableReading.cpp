//
// Created by Michał Zmyślony on 21/09/2021.
//

#include "TableReading.h"

#include <iostream>
#include <fstream>
#include <sstream>


std::vector<std::vector<double>> readFileToTableDouble(std::string &filename) {
    std::string line;
    std::ifstream file(filename);
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

//std::vector<std::vector<bool>> tableDoubleToBool(std::vector<std::vector<double>> &doubleTable) {
//    std::vector<std::vector<bool>> boolTable;
//    for (auto & row : doubleTable) {
//        std::vector<bool> boolRow;
//        boolRow.reserve(row.size());
//        for (auto & element : row) {
//            boolRow.push_back( (bool) element);
//        }
//        boolTable.push_back(boolRow);
//    }
//    return boolTable;
//}

//std::vector<std::vector<unsigned int>> tableDoubleToUint(std::vector<std::vector<double>> &doubleTable) {
//    std::vector<std::vector<unsigned int>> uintTable;
//    for (auto & row : doubleTable) {
//        std::vector<unsigned int> uintRow;
//        uintRow.reserve(row.size());
//        for (auto & element : row) {
//            uintRow.push_back((unsigned int) element);
//        }
//        uintTable.push_back(uintRow);
//    }
//    return uintTable;
//}

std::vector<std::vector<int>> readFileToTableInt(std::string &filename) {
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

