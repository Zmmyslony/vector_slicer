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

#ifndef VECTOR_SLICER_DISAGREEMENT_CONFIG_H
#define VECTOR_SLICER_DISAGREEMENT_CONFIG_H

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

class DisagreementConfig {
    int threads;
    int optimisation_seeds;
    int final_seeds;
    double agreement_percentile;
    int number_of_layers;
    bool is_disagreement_details_printed;

    std::string textDisagreementConfig() const;
public:
    explicit DisagreementConfig(const fs::path &config_path);

    DisagreementConfig(const fs::path &local_path, const fs::path &global_path);

    void saveDisagreementConfig(const fs::path &config_path);

    void saveDisagreementConfig(const fs::path &local_path, const fs::path &default_path);

    void printDisagreementConfig() const;

    void editDisagreementConfig();

    int getThreads() const;

    int getOptimisationSeeds() const;

    int getFinalSeeds() const;

    double getAgreementPercentile() const;

    int getNumberOfLayers() const;

    bool isDisagreementDetailsPrinted() const;
};


#endif //VECTOR_SLICER_DISAGREEMENT_CONFIG_H
