// 2022, Michał Zmyślony, mlz22@cam.ac.uk.
//
// Please cite Michał Zmyślony and Dr John Biggins if you use any part of this code in work you publish or distribute.
//
// This file is part of Vector Slicer.
//
// Vector Slicer is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//
// Vector Slicer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with Foobar. If not, see <https://www.gnu.org/licenses/>.

//
// Created by Michał Zmyślony on 10/01/2022.
//

#include "ConfigGeneration.h"
#include <iostream>


std::vector<FillingConfig> iterateOverOption(const DesiredPattern &desired_pattern, FillingConfig initial_config,
                                             double delta, int number_of_configs, configOptions option) {
    std::vector<FillingConfig> list_of_configs;
    double increment = delta / number_of_configs;
    double initial_value = stod(initial_config.getConfigOption(option));
    for (int i = -number_of_configs; i <= number_of_configs; i++) {
        double new_value = initial_value + i * increment;
        // TODO fix this condition to omit errors. Collision radius has to smaller than the print radius and greater
        // than zero so that the algorithm can work.
        if (option != CollisionRadius || new_value < initial_config.getPrintRadius() && new_value > 0) {

            initial_config.setConfigOption(option, std::to_string(new_value));
            list_of_configs.push_back(initial_config);
        }
    }
    return list_of_configs;
}


std::vector<FillingConfig> iterateOverSeeds(const DesiredPattern &desired_pattern, FillingConfig initial_config,
                                            int min_seed, int max_seed) {
    std::vector<FillingConfig> list_of_configs;
    for (int i = min_seed; i <= max_seed; i++) {
        initial_config.setConfigOption(Seed, std::to_string(i));
        list_of_configs.push_back(initial_config);
    }
    return list_of_configs;
}


std::vector<FillingConfig>
iterateOverSeeds(const DesiredPattern &desired_pattern, const std::vector<FillingConfig> &config_list, int min_seed,
                 int max_seed) {
    std::vector<FillingConfig> configs_to_test;
    for (auto &config_method: config_list) {
        std::vector<FillingConfig> configs_with_different_seeds = iterateOverSeeds(desired_pattern, config_method, min_seed,
                                                                                   max_seed);
        for (auto &config_seed: configs_with_different_seeds) {
            configs_to_test.push_back(config_seed);

        }
    }
    return configs_to_test;
}

