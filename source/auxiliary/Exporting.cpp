//
// Created by Michał Zmyślony on 13/10/2021.
//

#include "Exporting.h"
#include <iostream>
#include <fstream>
#include <sstream>

std::string readRowToString(const std::vector<int> &row) {
    std::string rowString;
    for (auto &element: row) {
        rowString += std::to_string(element);
        rowString += ",";
    }
    rowString.pop_back();
    rowString += "\n";
    return rowString;
}


std::string readRowToString(const std::vector<double> &row) {
    std::string rowString;
    for (auto &element: row) {
        rowString += std::to_string(element);
        rowString += ",";
    }
    rowString.pop_back();
    rowString += "\n";
    return rowString;
}


void exportVectorTableToFile(const std::vector<std::vector<int>> &table, std::string &filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        for (auto &row: table) {
            file << readRowToString(row);
        }
        file.close();
    }
}


void exportVectorTableToFile(const std::vector<std::vector<double>> &table, std::string &filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        for (auto &row: table) {
            file << readRowToString(row);
        }
    }
    file.close();
}


std::vector<std::vector<int>> indexTable(const std::vector<std::vector<std::valarray<int>>> &gridOfCoordinates,
                                         int index) {
    std::vector<std::vector<int>> table;
    table.reserve(gridOfCoordinates.size());
    for (auto &row: gridOfCoordinates) {
        std::vector<int> newRow;
        newRow.reserve(row.size());
        for (auto &element: row) {
            newRow.push_back(element[index]);
        }
        table.push_back(newRow);
    }
    return table;
}


void
export3DVectorToFile(const std::vector<std::vector<std::valarray<int>>> &gridOfCoordinates, const std::string &path,
                     const std::string &suffix) {
    std::vector<std::vector<int>> xTable = indexTable(gridOfCoordinates, 0);
    std::vector<std::vector<int>> yTable = indexTable(gridOfCoordinates, 1);

    std::string xFilename = path + "\\x_" + suffix + ".csv";
    std::string yFilename = path + "\\y_" + suffix + ".csv";

    exportVectorTableToFile(xTable, xFilename);
    exportVectorTableToFile(yTable, yFilename);
}


std::vector<std::vector<int>> importTableInt(const std::string &filename) {
    std::vector<std::vector<int>> table;
    std::string line;
    std::fstream file(filename);

    while (std::getline(file, line)) {
        std::string element;
        std::stringstream lineStream(line);
        std::vector<int> row;

        while (std::getline(lineStream, element, ',')) {
            row.push_back(stoi(element));
        }
        table.push_back(row);
    }
    return table;
}

std::vector<std::vector<std::valarray<int>>> mergeTwoTables(const std::vector<std::vector<int>> &xTable,
                                                            const std::vector<std::vector<int>> &yTable) {
    std::vector<std::vector<std::valarray<int>>> mergedTable;
    for (int i = 0; i < xTable.size(); i++) {
        std::vector<std::valarray<int>> mergedRow;
        for (int j = 0; j < xTable[i].size(); j++) {
            std::valarray<int> mergedElement = {xTable[i][j], yTable[i][j]};
            mergedRow.push_back(mergedElement);
        }
        mergedTable.push_back(mergedRow);
    }
    return mergedTable;
}


std::vector<std::vector<std::valarray<int>>> read3DVectorFromFile(const std::string &path, const std::string &suffix) {
    std::string xFilename = path + "\\x_" + suffix + ".csv";
    std::string yFilename = path + "\\y_" + suffix + ".csv";

    std::vector<std::vector<int>> xTable = importTableInt(xFilename);
    std::vector<std::vector<int>> yTable = importTableInt(yFilename);

    std::vector<std::vector<std::valarray<int>>> mergedTables = mergeTwoTables(xTable, yTable);
    return mergedTables;
}

