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
// Created by Michał Zmyślony on 21/11/2022.
//

#include <boost/filesystem/path.hpp>
#include <utility>
#include <chrono>

#include "bayesian_optimisation.h"
#include "../importing_and_exporting/OpenFiles.h"
#include "../importing_and_exporting/Exporting.h"
#include "../../ExecutionConfig.h"
#include "IndexedPath.h"
#include "../auxiliary/ProgressBar.h"

namespace fs = boost::filesystem;

BayesianOptimisation::BayesianOptimisation(QuantifiedConfig problem, int threads, int seeds,
                                           bayesopt::Parameters parameters) :
        ContinuousModel(4, std::move(parameters)),
        problem(std::move(problem)),
        threads(threads),
        seeds(seeds),
        begin(std::chrono::steady_clock::now()) {}


double BayesianOptimisation::evaluateSample(const vectord &x_in) {
    if (x_in.size() != 4) {
        std::cout << "WARNING: This only works for 4D inputs." << std::endl
                  << "WARNING: Using only first three components." << std::endl;
    }
    problem = QuantifiedConfig(problem, x_in);
    double disagreement = problem.getDisagreement(seeds, threads);
    showProgress(mCurrentIter, mParameters.n_iterations, begin);
    return disagreement;
}

void exportPatternToDirectory(FilledPattern pattern, const fs::path &pattern_path) {
    fs::path results_directory = pattern_path / "results";
    fs::path pattern_directory = pattern_path / "results";

    if (!fs::exists(results_directory)) {
        fs::create_directory(results_directory);
    }
    if (!fs::exists(pattern_directory)) {
        fs::create_directory(pattern_directory);
    }
    pattern.exportToDirectory(pattern_directory.string());

    std::vector<std::vector<std::valarray<int>>> sorted_paths = getSortedPaths(pattern, starting_point_number);
    export3DVectorToFile(sorted_paths, results_directory, "best_paths");
}


QuantifiedConfig generalOptimiser(int seeds, int threads, const DesiredPattern &desired_pattern,
                                  DisagreementWeights disagreement_weights, FillingConfig filling_config,
                                  bayesopt::Parameters optimisation_parameters) {

    QuantifiedConfig pattern(desired_pattern, filling_config, disagreement_weights);
    BayesianOptimisation pattern_optimisation(pattern, threads, seeds, std::move(optimisation_parameters));
    vectord best_config(4);
    vectord lower_bound(4);
    vectord upper_bound(4);

    double print_radius = pattern.getConfig().getPrintRadius();

    lower_bound[0] = 0; // Min repulsion
    lower_bound[1] = 1; // Min collision radius
    lower_bound[2] = print_radius; // Min starting point separation
    lower_bound[3] = 0; // Repulsion radius

    upper_bound[0] = 4;
    upper_bound[1] = print_radius + 1;
    upper_bound[2] = print_radius * 3;
    upper_bound[3] = print_radius;

    pattern_optimisation.setBoundingBox(lower_bound, upper_bound);
    pattern_optimisation.optimize(best_config);

    return {pattern, best_config};
}


void optimisePattern(const fs::path &pattern_path, int seeds, int threads) {
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    std::cout << "\n\nCurrent directory: " << pattern_path << std::endl;
    fs::path config_path = pattern_path / "config.txt";
    fs::path optimisation_log_path = pattern_path / "results" / "log.txt";

    FillingConfig initial_config(config_path);
    DesiredPattern desired_pattern = openPatternFromDirectory(pattern_path);
    DisagreementWeights default_weights(40, 2,
                                        8, 2,
                                        30, 2,
                                        0.05, 2);

//    QuantifiedConfig pattern(desired_pattern, initial_config, default_weights);

    bayesopt::Parameters parameters;
    parameters.random_seed = 0;
    parameters.l_type = L_MCMC;
    parameters.n_iterations = 190;
    parameters.n_iter_relearn = 20;
    parameters.noise = 1e-3;

    parameters.verbose_level = 4;
    parameters.log_filename = optimisation_log_path.string();

    QuantifiedConfig best_pattern = generalOptimiser(seeds, threads, desired_pattern, default_weights, initial_config,
                                                     parameters);
    QuantifiedConfig best_seed = best_pattern.findBestSeed(100, 8);

    exportPatternToDirectory(best_seed.getFilledPattern(), pattern_path);
    best_seed.getConfig().exportConfig(pattern_path);
    best_seed.getConfig().printConfig();
    best_seed.printDisagreement();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Execution time " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " s." << std::endl;
}

void fillPattern(const fs::path &pattern_path, const fs::path &config_path) {
    std::cout << "\n\nCurrent directory: " << pattern_path << std::endl;

    FillingConfig best_config(config_path);
    DesiredPattern desired_pattern = openPatternFromDirectory(pattern_path);
    DisagreementWeights default_weights(10, 2, 8, 2, 100, 2, 10, 2);

    QuantifiedConfig pattern(desired_pattern, best_config, default_weights);
    pattern.evaluate();

    exportPatternToDirectory(pattern.getFilledPattern(), pattern_path);
    pattern.getConfig().exportConfig(pattern_path / "results" / "best_config.txt");
}

void recalculateBestConfig(const fs::path &pattern_path) {
    fs::path config_path = pattern_path / "results" / "best_config.txt";
    fillPattern(pattern_path, config_path);
}