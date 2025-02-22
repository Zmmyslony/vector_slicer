// Copyright (c) 2021-2025, Michał Zmyślony, mlz22@cam.ac.uk.
//
// Please cite following publication if you use any part of this code in work you publish or distribute:
// [1] Michał Zmyślony M., Klaudia Dradrach, John S. Biggins,
//    Slicing vector fields into tool paths for additive manufacturing of nematic elastomers,
//    Additive Manufacturing, Volume 97, 2025, 104604, ISSN 2214-8604, https://doi.org/10.1016/j.addma.2024.104604.
//
// This file is part of Vector Slicer.
//
// Vector Slicer is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
// later version.
//
// Vector Slicer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License along with Vector Slicer.
// If not, see <https://www.gnu.org/licenses/>.

//
// Created by Michał Zmyślony on 27/09/2021.
//

#ifndef VECTOR_SLICER_TABLE_READING_H
#define VECTOR_SLICER_TABLE_READING_H

#include <vector>
#include <valarray>
#include <string>
#include "../coord.h"

std::vector<std::vector<double>> readFileToTableDouble(const std::string &filename);

std::vector<std::vector<uint8_t>> tableDoubleToInt(std::vector<std::vector<double>> &double_table);

std::vector<std::vector<uint8_t>> readFileToTableInt(const std::string &filename);

std::vector<int> getTableDimensions(std::string &filename);

std::vector<int> getTableDimensions(const std::vector<std::vector<uint8_t>> &table);

std::vector<std::vector<coord_d>> readFileToTableDoubleVector(const std::string &filename);

#endif //VECTOR_SLICER_TABLE_READING_H
