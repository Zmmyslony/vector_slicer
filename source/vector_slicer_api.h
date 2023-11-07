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
// Created by Michał Zmyślony on 24/10/2023.
//

#ifndef VECTOR_SLICER_VECTOR_SLICER_API_H
#define VECTOR_SLICER_VECTOR_SLICER_API_H

#include "vector_slicer_config.h"


/// Slices director pattern contained in pattern_directory using provided config from config_path
SLICER_DLLEXPORT void slice_pattern_with_config(const char *pattern_directory, const char *config_path);

/// Slices director pattern contained in pattern_directory and optimises it according to default penalty function
SLICER_DLLEXPORT void slice_pattern(const char *pattern_directory, bool is_default_used);



#endif //VECTOR_SLICER_VECTOR_SLICER_API_H
