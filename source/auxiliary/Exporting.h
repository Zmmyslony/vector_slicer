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
