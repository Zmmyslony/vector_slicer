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
// Created by Michał Zmyślony on 28/03/2023.
//

#ifndef VECTOR_SLICER_CONFIGURATION_READING_H
#define VECTOR_SLICER_CONFIGURATION_READING_H

#include <string>
#include <boost/filesystem.hpp>
#include <boost/dll.hpp>

namespace fs = boost::filesystem;

std::string readKey(const fs::path &file_path, const std::string &key);

int readKeyInt(const fs::path &file_path, const std::string &key);

int readKeyInt(const fs::path &local_file_path, const fs::path &default_file_path, const std::string &key);

double readKeyDouble(const fs::path &file_path, const std::string &key);

double readKeyDouble(const fs::path &local_file_path, const fs::path &default_file_path, const std::string &key);

bool readKeyBool(const fs::path &file_path, const std::string &key);

bool readKeyBool(const fs::path &local_file_path, const fs::path &default_file_path, const std::string &key);


#endif //VECTOR_SLICER_CONFIGURATION_READING_H
