//
// Created by Michał Zmyślony on 13/10/2021.
//

#ifndef VECTOR_SLICER_EXPORTING_H
#define VECTOR_SLICER_EXPORTING_H

#include <string>
#include <valarray>
#include <vector>

void exportVectorTableToFile(const std::vector<std::vector<int>> &table, std::string &filename);

void exportVectorTableToFile(const std::vector<std::vector<double>> &table, std::string &filename);

void export3DVectorToFile(const std::vector<std::vector<std::valarray<int>>> &gridOfCoordinates, const std::string &path,
                          const std::string &suffix);

std::vector<std::vector<std::valarray<int>>> read3DVectorFromFile(const std::string &path, const std::string &suffix);

#endif //VECTOR_SLICER_EXPORTING_H
