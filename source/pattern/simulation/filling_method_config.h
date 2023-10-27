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

public:
    explicit FillingMethodConfig(const fs::path &config_path);

    FillingMethodConfig(const fs::path &local_path, const fs::path &default_path);

    void saveFillingMethodConfig(const fs::path &config_path);

    bool isVectorFillingEnabled() const;

    bool isVectorSortingEnabled() const;

    bool isPointsRemoved() const;

    double getMinimalLineLength() const;
};


#endif //VECTOR_SLICER_FILLING_METHOD_CONFIG_H
