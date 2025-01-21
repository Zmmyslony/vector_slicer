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

#ifndef VECTOR_SLICER_BAYESIAN_OPTIMISATION_CONFIG_H
#define VECTOR_SLICER_BAYESIAN_OPTIMISATION_CONFIG_H

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

class BayesianOptimisationConfig {
    int total_iterations{};
    int improvement_iterations{};
    int relearning_iterations{};
    double noise{};
    int print_verbose{};

    bool is_collision_radius_optimised{};
    bool is_starting_point_separation_optimised{};
    bool is_repulsion_magnitude_optimised{};
    bool is_repulsion_angle_optimised{};

    std::string textBayesianOptimisationConfig() const;
public:

    explicit BayesianOptimisationConfig(const fs::path &config_path);

    BayesianOptimisationConfig(const fs::path &local_path, const fs::path &default_path);

    void saveBayesianOptimisationConfig(const fs::path &config_path);

    void saveBayesianOptimisationConfig(const fs::path &local_path, const fs::path &default_path);

    void printBayesianOptimisationConfig() const;

    void editBayesianOptimisationConfig();

    int getTotalIterations() const;

    int getImprovementIterations() const;

    int getRelearningIterations() const;

    double getNoise() const;

    int getPrintVerbose() const;

    bool isCollisionRadiusOptimised() const;

    bool isStartingPointSeparationOptimised() const;

    bool isRepulsionMagnitudeOptimised() const;

    bool isRepulsionAngleOptimised() const;
};


#endif //VECTOR_SLICER_BAYESIAN_OPTIMISATION_CONFIG_H
