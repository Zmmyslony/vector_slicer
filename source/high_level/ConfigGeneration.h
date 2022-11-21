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

#ifndef VECTOR_SLICER_CONFIGGENERATION_H
#define VECTOR_SLICER_CONFIGGENERATION_H

#include "../pattern/FillingConfig.h"
#include "../pattern/DesiredPattern.h"


std::vector<FillingConfig>
iterateOverSeeds(const DesiredPattern &desired_pattern, const std::vector<FillingConfig> &config_list, int min_seed,
                 int max_seed);

std::vector<FillingConfig>
iterateOverOption(const DesiredPattern &desired_pattern, FillingConfig initial_config, double delta, int number_of_configs,
                  configOptions option);

std::vector<FillingConfig>
iterateOverSeeds(const DesiredPattern &desired_pattern, FillingConfig initial_config, int min_seed, int max_seed);

#endif //VECTOR_SLICER_CONFIGGENERATION_H
