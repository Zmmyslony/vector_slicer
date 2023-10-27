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

public:
    explicit BayesianOptimisationConfig(const fs::path &config_path);

    BayesianOptimisationConfig(const fs::path &local_path, const fs::path &default_path);

    void saveToFile(const fs::path &config_path);

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
