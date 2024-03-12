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

#include "vector_slicer_api.h"
#include "bayesian_optimisation.h"

SLICER_DLLEXPORT void slice_pattern_with_config(const char *pattern_directory, const char *config_path) {
    fs::path pattern_path_fs(pattern_directory);
    fs::path config_path_fs(config_path);
    fillPattern(pattern_path_fs, config_path_fs);
}

SLICER_DLLEXPORT void slice_pattern(const char *pattern_directory, bool is_default_used) {
    fs::path pattern_path_fs(pattern_directory);
    optimisePattern(pattern_path_fs, is_default_used);
}

SLICER_DLLEXPORT void reslice_pattern(const char *pattern_directory) {
    fs::path pattern_path_fs(pattern_directory);
    recalculateBestConfig(pattern_path_fs);
}

