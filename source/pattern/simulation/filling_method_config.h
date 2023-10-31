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
// Created by Michał Zmyślony on 27/10/2023.
//

#ifndef VECTOR_SLICER_FILLING_METHOD_CONFIG_H
#define VECTOR_SLICER_FILLING_METHOD_CONFIG_H

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

class FillingMethodConfig {
    bool is_vector_filling_enabled;
    bool is_vector_sorting_enabled;
    bool is_points_removed;
    double minimal_line_length;

    std::string textFillingMethodConfig() const;
public:
    explicit FillingMethodConfig(const fs::path &config_path);

    FillingMethodConfig(const fs::path &local_path, const fs::path &default_path);

    void saveFillingMethodConfig(const fs::path &config_path);

    void saveFillingMethodConfig(const fs::path &local_path, const fs::path &default_path);

    void printFillingMethodConfig() const;

    void editFillingMethodConfig();

    bool isVectorFillingEnabled() const;

    bool isVectorSortingEnabled() const;

    bool isPointsRemoved() const;

    double getMinimalLineLength() const;
};


#endif //VECTOR_SLICER_FILLING_METHOD_CONFIG_H
