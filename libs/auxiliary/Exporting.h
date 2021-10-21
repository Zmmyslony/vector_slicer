//
// Created by Michał Zmyślony on 13/10/2021.
//

#ifndef VECTOR_SLICER_EXPORTING_H
#define VECTOR_SLICER_EXPORTING_H
#include <string>
#include <valarray>
#include <vector>

void exportVectorTableToFile(std::vector<std::vector<int>> table, std::string& filename);
void exportVectorTableToFile(std::vector<std::vector<double>>& table, std::string& filename);
#endif //VECTOR_SLICER_EXPORTING_H
