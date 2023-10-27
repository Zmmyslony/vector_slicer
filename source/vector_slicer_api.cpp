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

SLICER_DLLEXPORT void slice_pattern(const char *pattern_directory) {
    fs::path pattern_path_fs(pattern_directory);
    optimisePattern(pattern_path_fs);
}

