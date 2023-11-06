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

#include "filling_method_config.h"
#include "configuration_reading.h"
#include "interactive_input.h"

#include <iostream>
#include <sstream>
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


void FillingMethodConfig::printFillingMethodConfig() const {
    std::cout << textFillingMethodConfig();
}

void FillingMethodConfig::saveFillingMethodConfig(const fs::path &config_path) {
    std::ofstream file;
    file.open(config_path.string());
    file << textFillingMethodConfig();
    file.close();
}

void FillingMethodConfig::saveFillingMethodConfig(const fs::path &local_path, const fs::path &default_path) {
    bool is_saving_complete = false;
    while(!is_saving_complete) {
        std::cout << "Save the config? 0 - don't save, 1 - save locally, 2 - save as default" << std::endl;
        int saving_destination = readInt(0);
        switch(saving_destination) {
            case 0:
                std::cout << "Config is going to be not saved, do you confirm?" << std::endl;
                is_saving_complete = readBool(true);
                break;
            case 1:
                if(confirmation()) {
                    is_saving_complete = true;
                    saveFillingMethodConfig(local_path);
                }
                break;
            case 2:
                if(confirmation()) {
                    is_saving_complete = true;
                    saveFillingMethodConfig(default_path);
                }
                break;
            default:
                std::cout << "Invalid value! Select values from range 0-2" << std::endl;
                break;
        }
    }
}

std::string FillingMethodConfig::textFillingMethodConfig() const {
    std::ostringstream textForm;
    textForm << "# Switch between vector filling, where a path cannot continue if the desired director has inverted its direction and\n"
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
    return textForm.str();
}

void FillingMethodConfig::editFillingMethodConfig() {
    bool is_editing_finished = false;
    while (!is_editing_finished) {
        std::cout << "Editing filling method config." << std::endl;
        editBool(is_vector_filling_enabled, "is_vector_filling_enabled");
        editBool(is_vector_sorting_enabled, "is_vector_sorting_enabled");
        editBool(is_points_removed, "is_points_removed");
        editDouble(minimal_line_length, "minimal_line_length");

        std::cout << std::endl << "Current configuration:" << std::endl;
        printFillingMethodConfig();

        std::cout << std::endl << std::endl << "Finish editing? (default: true)";
        is_editing_finished = readBool(true);
    }
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
