//
// Created by Michał Zmyślony on 27/10/2023.
//

#include "filling_method_config.h"
#include "configuration_reading.h"
#include <iostream>
#include <fstream>

FillingMethodConfig::FillingMethodConfig(const fs::path &config_path) :
    is_vector_filling_enabled(readKeyBool(config_path, "is_vector_filling_enabled")),
    is_vector_sorting_enabled(readKeyBool(config_path, "is_vector_sorting_enabled")),
    is_points_removed(readKeyBool(config_path, "is_points_removed")),
    minimal_line_length(readKeyDouble(config_path, "minimal_line_length")) {

}

FillingMethodConfig::FillingMethodConfig(const fs::path &local_path, const fs::path &config_path) {
    if (fs::exists(local_path)) {
        std::cout << "Local filling method configuration file found." << std::endl;
        *this = FillingMethodConfig(local_path);
    } else {
        *this = FillingMethodConfig(config_path);
    }
}


void FillingMethodConfig::saveFillingMethodConfig(const fs::path &config_path) {
    std::ofstream file;
    file.open(config_path.string());
    file << "# Switch between vector filling, where a path cannot continue if the desired director has inverted its direction and\n"
            "# director operation where it does not matter."
         << "\nis_vector_filling_enabled = " << is_vector_filling_enabled
         << "\n\n# Switch for vector sorting, where the start and end points are distinguished through the vector field, and the director\n"
            "# sorting where a path can start from either direction."
         << "\nis_vector_sorting_enabled = " << is_vector_sorting_enabled
         << "\n\n# Switch for removing the points from the filled pattern, as they do not have the required directionality, but can\n"
            "# be used in order to fill the pattern more. It occurs before the disagreement calculation so the optimisation will try\n"
            "# to remove the holes existing after the removal of points."
         << "\nis_points_removed = " << is_points_removed
         << "\n\n# Lines shorter than minimal_line_length * print_radius will be removed. Happens before the disagreement calculation,\n"
            "# same as point removal."
         << "\nminimal_line_length = " << minimal_line_length;
    file.close();
}

bool FillingMethodConfig::isVectorFillingEnabled() const {
    return is_vector_filling_enabled;
}

bool FillingMethodConfig::isVectorSortingEnabled() const {
    return is_vector_sorting_enabled;
}

bool FillingMethodConfig::isPointsRemoved() const {
    return is_points_removed;
}

double FillingMethodConfig::getMinimalLineLength() const {
    return minimal_line_length;
}
