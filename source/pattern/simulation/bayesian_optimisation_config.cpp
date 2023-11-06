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

#include "bayesian_optimisation_config.h"
#include "configuration_reading.h"
#include "interactive_input.h"

#include <iostream>
#include <sstream>
#include <fstream>

BayesianOptimisationConfig::BayesianOptimisationConfig(const fs::path &config_path) {
    total_iterations = readKeyInt(config_path, "number_of_iterations");
    improvement_iterations = readKeyInt(config_path, "number_of_improvement_iterations");
    relearning_iterations = readKeyInt(config_path, "iterations_between_relearning");
    noise = readKeyDouble(config_path, "noise");
    print_verbose = readKeyInt(config_path, "print_verbose");

    is_collision_radius_optimised = readKeyBool(config_path, "is_collision_radius_optimised");
    is_starting_point_separation_optimised = readKeyBool(config_path, "is_starting_point_separation_optimised");
    is_repulsion_magnitude_optimised = readKeyBool(config_path, "is_repulsion_magnitude_optimised");
    is_repulsion_angle_optimised = readKeyBool(config_path, "is_repulsion_angle_optimised");
}


BayesianOptimisationConfig::BayesianOptimisationConfig(const fs::path &local_path, const fs::path &config_path) {
    if (fs::exists(local_path)) {
        std::cout << "Local Bayesian optimisation configuration file found." << std::endl;
        *this = BayesianOptimisationConfig(local_path);
    } else {
        *this = BayesianOptimisationConfig(config_path);
    }
}


std::string BayesianOptimisationConfig::textBayesianOptimisationConfig() const {
    std::ostringstream textForm;
    textForm << "# Number of sets of generating parameters to iterate over in Bayesian optimisation."
             << "\nnumber_of_iteration = " << total_iterations
             << "\n\n# If the disagreement does not improve in this many iterations, then the algorithm finishes. Set to 0 to disable it."
             << "\nnumber_of_improvement_iterations = " << improvement_iterations
             << "\n\n# Number of iterations after which the model relearns based on all previous data points."
             << "\niterations_between_relearning = " << relearning_iterations
             << "\n\n# Assumed data noise. Larger values looks for new global minima while lower values probes the local minimum more precisely."
             << "\nnoise = " << noise
             << "\n\n# How to print logs. - (0 - error, 1 - info, 2 - debug) to std::cout, (3, 4, 5) same but to log.txt"
             << "\nprint_verbose = " << print_verbose
             << "\n\n# Settings to choose which parameters are optimised"
             << "\nis_collision_radius_optimised = " << is_collision_radius_optimised
             << "\nis_starting_point_separation_optimised = " << is_starting_point_separation_optimised
             << "\nis_repulsion_magnitude_optimised = " << is_repulsion_magnitude_optimised
             << "\nis_repulsion_angle_optimised = " << is_repulsion_angle_optimised;
    return textForm.str();
}

void BayesianOptimisationConfig::printBayesianOptimisationConfig() const {
    std::cout << textBayesianOptimisationConfig();
}

void BayesianOptimisationConfig::saveBayesianOptimisationConfig(const fs::path &config_path) {
    std::ofstream file;
    file.open(config_path.string());
    file << textBayesianOptimisationConfig();
    file.close();
}

void BayesianOptimisationConfig::saveBayesianOptimisationConfig(const fs::path &local_path, const fs::path &default_path) {
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
                    saveBayesianOptimisationConfig(local_path);
                }
                break;
            case 2:
                if(confirmation()) {
                    is_saving_complete = true;
                    saveBayesianOptimisationConfig(default_path);
                }
                break;
            default:
                std::cout << "Invalid value! Select values from range 0-2" << std::endl;
                break;
        }
    }
}

void BayesianOptimisationConfig::editBayesianOptimisationConfig() {
    bool is_editing_finished = false;
    while (!is_editing_finished) {
        std::cout << "Editing Bayesian optimisation config." << std::endl;
        editInt(total_iterations, "total_iterations");
        editInt(improvement_iterations, "improvement_iterations");
        editInt(relearning_iterations, "relearning_iterations");
        editDouble(noise, "noise");
        editInt(print_verbose, "print_verbose");
        editBool(is_collision_radius_optimised, "is_collision_radius_optimised");
        editBool(is_starting_point_separation_optimised, "is_starting_point_separation_optimised");
        editBool(is_repulsion_magnitude_optimised, "is_repulsion_magnitude_optimised");
        editBool(is_repulsion_angle_optimised, "is_repulsion_angle_optimised");

        std::cout << std::endl << "Current configuration:" << std::endl;
        printBayesianOptimisationConfig();

        std::cout << std::endl << std::endl << "Finish editing? (default: true)";
        is_editing_finished = readBool(true);
    }
}

int BayesianOptimisationConfig::getTotalIterations() const {
    return total_iterations;
}

int BayesianOptimisationConfig::getImprovementIterations() const {
    return improvement_iterations;
}

int BayesianOptimisationConfig::getRelearningIterations() const {
    return relearning_iterations;
}

double BayesianOptimisationConfig::getNoise() const {
    return noise;
}

int BayesianOptimisationConfig::getPrintVerbose() const {
    return print_verbose;
}

bool BayesianOptimisationConfig::isCollisionRadiusOptimised() const {
    return is_collision_radius_optimised;
}

bool BayesianOptimisationConfig::isStartingPointSeparationOptimised() const {
    return is_starting_point_separation_optimised;
}

bool BayesianOptimisationConfig::isRepulsionMagnitudeOptimised() const {
    return is_repulsion_magnitude_optimised;
}

bool BayesianOptimisationConfig::isRepulsionAngleOptimised() const {
    return is_repulsion_angle_optimised;
}
