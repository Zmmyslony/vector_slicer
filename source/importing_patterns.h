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
// Created by Michał Zmyślony on 07/12/2022.
//

#ifndef VECTOR_SLICER_IMPORTING_PATTERNS_H
#define VECTOR_SLICER_IMPORTING_PATTERNS_H

#include <vector>
#include <boost/filesystem.hpp>
#include <boost/dll.hpp>

namespace fs = boost::filesystem;

std::vector<fs::path> getPatterns(const fs::path &list_of_patterns_path);

#endif //VECTOR_SLICER_IMPORTING_PATTERNS_H
