// Copyright (c) 2023, Michał Zmyślony, mlz22@cam.ac.uk.
//
// Please cite Michał Zmyślony and Dr John Biggins if you use any part of this code in work you publish or distribute.
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
// Created by Michał Zmyślony on 24/09/2021.
//

#ifndef VECTOR_SLICER_OPEN_FILES_H
#define VECTOR_SLICER_OPEN_FILES_H

#include "../filled_pattern.h"

#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/dll.hpp>

namespace fs = boost::filesystem;

DesiredPattern openPatternFromDirectory(const fs::path &directory_path, bool is_splay_filling_enabled, int threads,
                                        const FillingMethodConfig &filling);

FilledPattern openFilledPatternFromDirectory(const fs::path &directory_path, int threads);

FilledPattern openFilledPatternFromDirectory(const fs::path &directory_path, unsigned int seed, int threads);

std::vector<int> readConfigTable(const fs::path &config_path);

FilledPattern
openFilledPatternFromDirectoryAndPattern(const fs::path &directory_path, const DesiredPattern &pattern, unsigned int seed);

FilledPattern openFilledPatternFromDirectoryAndPattern(const fs::path &directory_path, const DesiredPattern &pattern);

#endif //VECTOR_SLICER_OPEN_FILES_H
