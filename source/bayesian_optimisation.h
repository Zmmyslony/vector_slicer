
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
// You should have received a copy of the GNU General Public License along with Vector Slicer. If not, see <https://www.gnu.org/licenses/>.

//
// Created by Michał Zmyślony on 21/11/2022.
//

#ifndef VECTOR_SLICER_BAYESIAN_OPTIMISATION_H
#define VECTOR_SLICER_BAYESIAN_OPTIMISATION_H

#include "bayesopt/bayesopt.hpp"
#include <utility>
#include "pattern/QuantifiedConfig.h"

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

public:
    BayesianOptimisation(QuantifiedConfig problem, int threads, int seeds,
                         bayesopt::Parameters parameters, int dims);

    /// Evaluates current FillingConfig which is defined and returns disagreement
    double evaluateSample(const vectord &x_in);

    bool checkReachability(const vectord &query) { return true; };

};

/// Optimises the pattern from the selected path using the set configuration
void optimisePattern(const fs::path &pattern_path, int seeds, int threads);

/// Optimises the pattern from the selected path using .cfg configuration
void optimisePattern(const fs::path &pattern_path);

/// Recalculates the best config using a config from output/best_configs
void recalculateBestConfig(const fs::path &pattern_path);

#endif //VECTOR_SLICER_BAYESIAN_OPTIMISATION_H
