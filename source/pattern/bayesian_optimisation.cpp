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
#include "bayesian_optimisation.h"

#include "../importing_and_exporting/OpenFiles.h"
#include "../importing_and_exporting/Exporting.h"
#include "../../ExecutionConfig.h"
#include "IndexedPath.h"

namespace fs = boost::filesystem;

BayesianOptimisation::BayesianOptimisation(QuantifiedConfig problem, int threads, int seeds,
                                           bayesopt::Parameters parameters) :
        ContinuousModel(3, std::move(parameters)),
        problem(std::move(problem)),
        threads(threads),
        seeds(seeds) {}


double BayesianOptimisation::evaluateSample(const vectord &x_in) {
    if (x_in.size() != 3) {
        std::cout << "WARNING: This only works for 3D inputs." << std::endl
                  << "WARNING: Using only first three components." << std::endl;
    }
    problem = QuantifiedConfig(problem, x_in);
    return problem.getDisagreement(seeds, threads);
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


void generalOptimiser(int seeds, int threads, const DesiredPattern& desired_pattern, DisagreementWeights disagreement_weights, FillingConfig filling_config, bayesopt::Parameters optimisation_parameters) {

    QuantifiedConfig pattern(desired_pattern, filling_config, disagreement_weights);
    BayesianOptimisation pattern_optimisation(pattern, threads, seeds, std::move(optimisation_parameters));
    vectord best_config(3);
    vectord lower_bound(3);
    vectord upper_bound(3);
    lower_bound[0] = 0; // Min repulsion
    lower_bound[1] = 0; // Min collision radius
    lower_bound[2] = pattern.getConfig().getPrintRadius() * 0.5; // Min starting point separation

    upper_bound[0] = 2;
    upper_bound[1] = pattern.getConfig().getPrintRadius() * 1.5;
    upper_bound[2] = pattern.getConfig().getPrintRadius() * 3;

    pattern_optimisation.setBoundingBox(lower_bound, upper_bound);
    pattern_optimisation.optimize(best_config);

    QuantifiedConfig optimised_pattern(pattern, best_config);
}


void generalFinder(const fs::path &pattern_path, int seeds, int threads) {
    time_t start_time = clock();
    std::cout << "\n\nCurrent directory: " << pattern_path << std::endl;
    fs::path config_path = pattern_path / "config.txt";
    fs::path optimisation_log_path = pattern_path / "results" / "log.txt";

    FillingConfig initial_config(config_path);
    DesiredPattern desired_pattern = openPatternFromDirectory(pattern_path);
    DisagreementWeights default_weights(40, 2,
                                        8, 2,
                                        30, 2,
                                        0.05, 2);

    QuantifiedConfig pattern(desired_pattern, initial_config, default_weights);

    bayesopt::Parameters parameters;
    parameters.l_type = L_MCMC;
    parameters.n_iterations = 100;
//    parameters.l_type = L_EMPIRICAL;
    parameters.noise = 1e-3;
    parameters.n_iter_relearn = 20;
    parameters.log_filename = optimisation_log_path.string();
    BayesianOptimisation pattern_optimisation(pattern, threads, seeds, parameters);
    vectord best_config(3);
    vectord lower_bound(3);
    vectord upper_bound(3);
    lower_bound[0] = 0; // Min repulsion
    lower_bound[1] = 1; // Min collision radius
    lower_bound[2] = pattern.getConfig().getPrintRadius(); // Min starting point separation

    upper_bound[0] = 2;
    upper_bound[1] = pattern.getConfig().getPrintRadius() + 1;
    upper_bound[2] = pattern.getConfig().getPrintRadius() * 3;

    pattern_optimisation.setBoundingBox(lower_bound, upper_bound);
    pattern_optimisation.optimize(best_config);

    QuantifiedConfig best_pattern = QuantifiedConfig(pattern, best_config).findBestSeed(100, 8);

    exportPatternToDirectory(best_pattern.getFilledPattern(), pattern_path);
    best_pattern.getConfig().exportConfig(pattern_path);
    best_pattern.getConfig().printConfig();
    best_pattern.printDisagreement();
    printf("Multi-thread execution time %.2f", (double) (clock() - start_time) / CLOCKS_PER_SEC);
}

void calculatePattern(const fs::path &pattern_path, const fs::path &config_path) {
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
    calculatePattern(pattern_path, config_path);
}