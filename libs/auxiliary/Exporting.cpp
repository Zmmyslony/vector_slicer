//
// Created by Michał Zmyślony on 13/10/2021.
//

#include "Exporting.h"
#include <iostream>
#include <fstream>

std::string readRowToString(std::vector<int>& row) {
    std::string rowString;
    for (auto& element : row) {
        rowString += std::to_string(element);
        rowString += ",";
    }
    rowString.pop_back();
    rowString += "\n";
    return rowString;
}


std::string readRowToString(std::vector<double>& row) {
    std::string rowString;
    for (auto& element : row) {
        rowString += std::to_string(element);
        rowString += ",";
    }
    rowString.pop_back();
    rowString += "\n";
    return rowString;
}


void exportVectorTableToFile(std::vector<std::vector<int>> table, std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        for (auto& row : table) {
            file << readRowToString(row);
        }
        file.close();
    }
}


void exportVectorTableToFile(std::vector<std::vector<double>>& table, std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        for (auto& row : table) {
            file << readRowToString(row);
        }
    }
    file.close();
}
