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

#define DISCONTINUITY_IGNORE 0
#define DISCONTINUITY_STICK 1
#define DISCONTINUITY_TERMINATE 2

namespace fs = boost::filesystem;

class FillingMethodConfig {
    bool is_vector_filling_enabled = false;
    bool is_vector_sorting_enabled = false;
    bool is_points_removed = true;
    double minimal_line_length;
    double discontinuity_angular_threshold = 40;
    int discontinuity_behaviour = DISCONTINUITY_STICK;
    int sorting_method = 0;

public:
    std::string textFillingMethodConfig() const;

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

    double getDiscontinuityThreshold() const;

    int getDiscontinuityBehaviour() const;

    int getSortingMethod() const;
};


#endif //VECTOR_SLICER_FILLING_METHOD_CONFIG_H
