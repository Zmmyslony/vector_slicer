//
// Created by Michał Zmyślony on 24/10/2023.
//

#ifndef VECTOR_SLICER_VECTOR_SLICER_API_H
#define VECTOR_SLICER_VECTOR_SLICER_API_H

#include "vector_slicer_config.h"


/// Slices director pattern contained in pattern_directory using provided config from config_path
SLICER_DLLEXPORT void slice_pattern_with_config(const char *pattern_directory, const char *config_path);

/// Slices director pattern contained in pattern_directory and optimises it according to default penalty function
SLICER_DLLEXPORT void slice_pattern(const char *pattern_directory);



#endif //VECTOR_SLICER_VECTOR_SLICER_API_H
