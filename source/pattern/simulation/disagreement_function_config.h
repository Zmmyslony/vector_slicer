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

#ifndef VECTOR_SLICER_DISAGREEMENT_FUNCTION_CONFIG_H
#define VECTOR_SLICER_DISAGREEMENT_FUNCTION_CONFIG_H

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

class DisagreementFunctionConfig {
private:
    double empty_spot_weight;
    double empty_spot_power;
    double overlap_weight;
    double overlap_power;
    double director_weight;
    double director_power;
    double paths_power;

    std::string textDisagreementFunctionConfig() const;
public:
    explicit DisagreementFunctionConfig(const fs::path &config_path);

    DisagreementFunctionConfig(const fs::path &local_path, const fs::path &default_path);

    void saveDisagreementFunctionConfig(const fs::path &config_path);

    void saveDisagreementFunctionConfig(const fs::path &local_path, const fs::path &default_path);

    void printDisagreementFunctionConfig() const;

    void editDisagreementFunctionConfig();

    double getEmptySpotWeight() const;

    double getEmptySpotPower() const;

    double getOverlapWeight() const;

    double getOverlapPower() const;

    double getDirectorWeight() const;

    double getDirectorPower() const;

    double getPathsPower() const;
};


#endif //VECTOR_SLICER_DISAGREEMENT_FUNCTION_CONFIG_H
