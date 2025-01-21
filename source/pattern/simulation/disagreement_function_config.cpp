// Copyright (c) 2023-2025, Michał Zmyślony, mlz22@cam.ac.uk.
//
// Please cite following publication if you use any part of this code in work you publish or distribute:
// [1] Michał Zmyślony M., Klaudia Dradrach, John S. Biggins,
//    Slicing vector fields into tool paths for additive manufacturing of nematic elastomers,
//    Additive Manufacturing, Volume 97, 2025, 104604, ISSN 2214-8604, https://doi.org/10.1016/j.addma.2024.104604.
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

#include "disagreement_function_config.h"
#include "configuration_reading.h"
#include "interactive_input.h"

#include <iostream>
#include <sstream>
#include <fstream>


DisagreementFunctionConfig::DisagreementFunctionConfig(const fs::path &config_path) :
        empty_spot_weight(readKeyDouble(config_path, "empty_spot_weight")),
        empty_spot_power(readKeyDouble(config_path, "empty_spot_power")),
        overlap_weight(readKeyDouble(config_path, "overlap_weight")),
        overlap_power(readKeyDouble(config_path, "overlap_power")),
        director_weight(readKeyDouble(config_path, "director_weight")),
        director_power(readKeyDouble(config_path, "director_power")),
        paths_power(readKeyDouble(config_path, "paths_power")) {
}

DisagreementFunctionConfig::DisagreementFunctionConfig(const fs::path &local_path, const fs::path &config_path) {
    if (fs::exists(local_path)) {
        std::cout << "Local disagreement function configuration file found." << std::endl;
        *this = DisagreementFunctionConfig(local_path);
    } else {
        *this = DisagreementFunctionConfig(config_path);
    }
}

void DisagreementFunctionConfig::saveDisagreementFunctionConfig(const fs::path &config_path) {
    std::ofstream file;
    file.open(config_path.string());
    file << textDisagreementFunctionConfig();
    file.close();
}

void
DisagreementFunctionConfig::saveDisagreementFunctionConfig(const fs::path &local_path, const fs::path &default_path) {
    bool is_saving_complete = false;
    while (!is_saving_complete) {
        std::cout << "Save the config? 0 - don't save, 1 - save locally, 2 - save as default" << std::endl;
        int saving_destination = readInt(0);
        switch (saving_destination) {
            case 0:
                std::cout << "Config is going to be not saved, do you confirm?" << std::endl;
                is_saving_complete = readBool(true);
                break;
            case 1:
                if (confirmation()) {
                    is_saving_complete = true;
                    saveDisagreementFunctionConfig(local_path);
                }
                break;
            case 2:
                if (confirmation()) {
                    is_saving_complete = true;
                    saveDisagreementFunctionConfig(default_path);
                }
                break;
            default:
                std::cout << "Invalid value! Select values from range 0-2" << std::endl;
                break;
        }
    }
}

std::string DisagreementFunctionConfig::textDisagreementFunctionConfig() const {
    std::ostringstream textForm;
    textForm
            << "# The value of the disagreement of the pattern is a value of a polynomial with following weights and powers."
            << "\n# By tweaking the relative value of these parameters user can penalise varying effects."
            << "\nempty_spot_weight = " << empty_spot_weight
            << "\nempty_spot_power = " << empty_spot_power
            << "\noverlap_weight = " << overlap_weight
            << "\noverlap_power = " << overlap_power
            << "\ndirector_weight = " << director_weight
            << "\ndirector_power = " << director_power
            << "\n\n# The only multiplicative factor proportional to the number of paths risen to the correct power"
            << "\npaths_power = " << paths_power;
    return textForm.str();
}

void DisagreementFunctionConfig::printDisagreementFunctionConfig() const {
    std::cout << textDisagreementFunctionConfig() << std::endl;
}

void DisagreementFunctionConfig::editDisagreementFunctionConfig() {
    bool is_editing_finished = false;
    while (!is_editing_finished) {
        std::cout << "Editing disagreement function config." << std::endl;
        editDouble(empty_spot_weight, "empty_spot_weight");
        editDouble(empty_spot_power, "empty_spot_power");
        editDouble(overlap_weight, "overlap_weight");
        editDouble(overlap_power, "overlap_power");
        editDouble(director_weight, "director_weight");
        editDouble(director_power, "director_power");
        editDouble(paths_power, "paths_power");

        printDisagreementFunctionConfig();

        std::cout << std::endl << std::endl << "Finish editing? (default: true)";
        is_editing_finished = readBool(true);
    }
}

double DisagreementFunctionConfig::getEmptySpotWeight() const {
    return empty_spot_weight;
}

double DisagreementFunctionConfig::getEmptySpotPower() const {
    return empty_spot_power;
}

double DisagreementFunctionConfig::getOverlapWeight() const {
    return overlap_weight;
}

double DisagreementFunctionConfig::getOverlapPower() const {
    return overlap_power;
}

double DisagreementFunctionConfig::getDirectorWeight() const {
    return director_weight;
}

double DisagreementFunctionConfig::getDirectorPower() const {
    return director_power;
}

double DisagreementFunctionConfig::getPathsPower() const {
    return paths_power;
}
