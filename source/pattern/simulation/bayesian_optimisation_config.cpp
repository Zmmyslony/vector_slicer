//
// Created by Michał Zmyślony on 27/10/2023.
//

#include "bayesian_optimisation_config.h"
#include "configuration_reading.h"
#include <iostream>
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

void BayesianOptimisationConfig::saveToFile(const fs::path &config_path) {
    std::ofstream file;
    file.open(config_path.string());
    file << "# Number of sets of generating parameters to iterate over in Bayesian optimisation."
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
         << "\nis_collision_radius_optimised" << is_collision_radius_optimised
         << "\nis_starting_point_separation_optimised" << is_starting_point_separation_optimised
         << "\nis_repulsion_magnitude_optimised" << is_repulsion_magnitude_optimised
         << "\nis_repulsion_angle_optimised" << is_repulsion_angle_optimised;
    file.close();
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
