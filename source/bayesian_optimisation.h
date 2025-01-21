// Copyright (c) 2022-2025, Michał Zmyślony, mlz22@cam.ac.uk.
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
// Created by Michał Zmyślony on 21/11/2022.
//

#ifndef VECTOR_SLICER_BAYESIAN_OPTIMISATION_H
#define VECTOR_SLICER_BAYESIAN_OPTIMISATION_H

#include "bayesopt/bayesopt.hpp"
#include <utility>
#include "pattern/quantified_config.h"

/// \brief The main class responsible for the optimisation of the problem. The QuantifiedConfig contains all the information
/// about the DesiredPattern and the DisagreementWeights that will be used for optimisation.
class BayesianOptimisation : public bayesopt::ContinuousModel {
    QuantifiedConfig problem;
    std::chrono::steady_clock::time_point begin;
    int threads;
    int seeds;
    int dims;
    bool is_disagreement_details_printed = false;
    double disagreement_percentile = 0.5;
    long evaluation_time_ns = 0;
public:
    long getEvaluationTimeNs() const;

private:

    bool is_repulsion_angle_optimised = true;
    bool is_repulsion_magnitude_optimised = true;
    bool is_starting_point_separation_optimised = true;
    bool is_collision_radius_optimised = true;

    void showProgress(int current_step, int max_step, int steps_from_improvement, int steps_threshold,
                      int step_offset);
    /// Evaluates current FillingConfig which is defined and returns disagreement
    double evaluateSample(const vectord &x_in);

    bool checkReachability(const vectord &query) { return true; };

    void evaluateGuesses(const std::vector<std::vector<double>>& fixed_guesses);
public:

    BayesianOptimisation(QuantifiedConfig problem, bayesopt::Parameters parameters, int dims);


    /// Optimizes the pattern with a threshold on number of steps without improvement
    void optimizeControlled(vectord &x_out, int max_steps, int max_constant_steps,
                            const std::vector<std::vector<double>>& fixed_guesses);
};

/// Optimises the pattern from the selected path using the set configuration
void optimisePattern(const fs::path &pattern_path, int seeds, int threads);

/// Optimises the pattern from the selected path using .cfg configuration
void optimisePattern(const fs::path &pattern_path, bool is_default_used);

/// Fills a pattern using provided config.
void fillPattern(const fs::path &pattern_path, const fs::path &config_path);

/// Recalculates the best config using a config from output/best_configs
void recalculateBestConfig(const fs::path &pattern_path);

/// Scans a certain number of seeds and finds the best one.
void optimisePatternSeeds(const fs::path &pattern_path, const fs::path &config_path, int seeds);
#endif //VECTOR_SLICER_BAYESIAN_OPTIMISATION_H
