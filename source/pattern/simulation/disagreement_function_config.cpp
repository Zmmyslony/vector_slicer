//
// Created by Michał Zmyślony on 27/10/2023.
//

#include "disagreement_function_config.h"
#include "configuration_reading.h"
#include <iostream>
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
    file
            << "# The value of the disagreement of the pattern is a value of a polynomial with following weights and powers."
            << "\n# By tweaking the relative value of these parameters user can penalise varying effects."
            << "\nempty_spot_weight = " << empty_spot_weight
            << "\nempty_spot_power = " << empty_spot_power
            << "\noverlap_weight = " << overlap_weight
            << "\noverlap_power = " << overlap_power
            << "\ndirector_weight" << director_weight
            << "\ndirector_power" << director_power
            << "\n\n# The only multiplicative factor proportional to the number of paths risen to the correct power"
            << "\npaths_power" << paths_power;
    file.close();
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
