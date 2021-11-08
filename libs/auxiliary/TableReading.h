//
// Created by Michał Zmyślony on 27/09/2021.
//

#ifndef VECTOR_SLICER_TABLEREADING_H
#define VECTOR_SLICER_TABLEREADING_H

#include <vector>
#include <valarray>
#include <string>

std::vector<std::vector<double>> readFileToTableDouble(std::string &filename);

std::vector<std::vector<int>> tableDoubleToInt(std::vector<std::vector<double>> &doubleTable);

std::vector<std::vector<int>> readFileToTableInt(std::string &filename);

std::valarray<int> getTableDimensions(std::string &filename);


#endif //VECTOR_SLICER_TABLEREADING_H
