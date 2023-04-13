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

#include <boost/filesystem/path.hpp>
#include <utility>
#include <chrono>

#include "bayesian_optimisation.h"
#include "../ExecutionConfig.h"
#include "vector_slicer_config.h"
#include "pattern/DirectorIndexedPath.h"
#include "pattern/vector_sorted_paths.h"
#include "pattern/importing_and_exporting/OpenFiles.h"
#include "pattern/importing_and_exporting/Exporting.h"
#include "pattern/auxiliary/ProgressBar.h"
#include "pattern/auxiliary/configuration_reading.h"

namespace fs = boost::filesystem;

BayesianOptimisation::BayesianOptimisation(QuantifiedConfig problem, int threads, int seeds,
                                           bayesopt::Parameters parameters, int dims) :
        ContinuousModel(dims, std::move(parameters)),
        dims(dims),
        problem(std::move(problem)),
        threads(threads),
        seeds(seeds),
        begin(std::chrono::steady_clock::now()),
        is_disagreement_details_printed(readKeyBool(DISAGREEMENT_CONFIG, "is_disagreement_details_printed")) {
    // Needed, as after optimisation of the first pattern the mCurrentIter does not reset, while getValueAtMinium
    // does not have an object to refer to, so it tries to access an out-of-scope object and crashes.
    mCurrentIter = 0;
}


double BayesianOptimisation::evaluateSample(const vectord &x_in) {
    if (x_in.size() != dims) {
        std::cerr << "ERROR: This only works for " << dims << "D inputs." << std::endl
                  << "ERROR: Using only first four components." << std::endl;
    }
    problem = QuantifiedConfig(problem, x_in, dims);
    double disagreement = problem.getDisagreement(seeds, threads, is_disagreement_details_printed);
    if (mCurrentIter > 0) {
        double minimal_disagreement = getValueAtMinimum();
        showProgress(mCurrentIter + mParameters.n_init_samples, mParameters.n_iterations + mParameters.n_init_samples,
                     begin, minimal_disagreement);
    }
    return disagreement;
}

void exportPatternToDirectory(FilledPattern pattern, const fs::path &pattern_path) {
    fs::path results_directory = pattern_path / "results";
    fs::path generated_paths_directory = pattern_path.parent_path().parent_path() / "paths";

    if (!fs::exists(results_directory)) {
        fs::create_directory(results_directory);
    }
    pattern.exportFilledMatrix(results_directory.string());
    std::vector<std::vector<std::valarray<int>>> sorted_paths;

    if (pattern.desired_pattern.get().isVectorFillingEnabled()) {
        sorted_paths = getVectorSortedPaths(pattern.getSequenceOfPaths(), {0, 0});
    } else {
        sorted_paths = getDirectorSortedPaths(pattern, starting_point_number);
    }
    exportPathSequence(sorted_paths, results_directory, "best_paths", pattern.getPrintRadius() * 2 + 1);
    exportPathSequence(sorted_paths, generated_paths_directory, pattern_path.stem().string(),
                       pattern.getPrintRadius() * 2 + 1);
}

void exportPatternsToDirectory(const std::vector<QuantifiedConfig> &patterns, const fs::path &pattern_path) {
    fs::path results_directory = pattern_path / "results";
    fs::path generated_paths_directory = pattern_path.parent_path().parent_path() / "paths";

    if (!fs::exists(results_directory)) {
        fs::create_directory(results_directory);
    }

    patterns[0].getFilledPattern().exportFilledMatrix(results_directory.string());

    std::vector<std::vector<std::vector<std::valarray<int>>>> sorted_paths;
    double print_diameter = 0;
    for (int i = 0; i < 10; i++) {
        FilledPattern pattern = patterns[i].getFilledPattern();
        if (pattern.desired_pattern.get().isVectorFillingEnabled()) {
            sorted_paths.emplace_back(getVectorSortedPaths(pattern.getSequenceOfPaths(), {0, 0}));
        } else {
            sorted_paths.emplace_back(getDirectorSortedPaths(pattern, starting_point_number));
        }
        print_diameter = pattern.getPrintRadius() * 2 + 1;
    }

    exportPathSequence(sorted_paths, results_directory, "best_paths", print_diameter);
    exportPathSequence(sorted_paths, generated_paths_directory, pattern_path.stem().string(),
                       print_diameter);
}


QuantifiedConfig generalOptimiser(int seeds, int threads, const DesiredPattern &desired_pattern,
                                  DisagreementWeights disagreement_weights, FillingConfig filling_config,
                                  bayesopt::Parameters optimisation_parameters, int dims) {

    QuantifiedConfig pattern(desired_pattern, filling_config, disagreement_weights);
    BayesianOptimisation pattern_optimisation(pattern, threads, seeds, std::move(optimisation_parameters), dims);
    vectord best_config(dims);
    vectord lower_bound(dims);
    vectord upper_bound(dims);

    double print_radius = pattern.getConfig().getPrintRadius();

    lower_bound[0] = 0; // Min repulsion
    lower_bound[1] = 1; // Min collision radius
    lower_bound[2] = print_radius * 1.5; // Min starting point separation

    upper_bound[0] = 4;
    upper_bound[1] = print_radius + 1;
    upper_bound[2] = print_radius * 2.5;

    if (dims > 3) {
        lower_bound[3] = 0; // RepulsionRadius
        upper_bound[3] = print_radius;
    }

    pattern_optimisation.setBoundingBox(lower_bound, upper_bound);
    pattern_optimisation.optimize(best_config);

    return {pattern, best_config, 3};
}


void optimisePattern(const fs::path &pattern_path, int seeds, int threads) {
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    std::cout << "\n\nCurrent directory: " << pattern_path << std::endl;
    fs::path config_path = pattern_path / "config.txt";
    fs::path optimisation_log_path = pattern_path / "results" / "log.txt";
    fs::path optimisation_save_path = pattern_path / "results" / "save.txt";

    FillingConfig initial_config(config_path);
    DesiredPattern desired_pattern = openPatternFromDirectory(pattern_path);
    DisagreementWeights weights(DISAGREEMENT_FUNCTION_CONFIG);

    bayesopt::Parameters parameters;
    parameters.random_seed = 0;
    parameters.l_type = L_MCMC;
    parameters.n_iterations = readKeyInt(BAYESIAN_CONFIG, "number_of_iterations");
    parameters.n_iter_relearn = readKeyInt(BAYESIAN_CONFIG, "iterations_between_relearning");
    parameters.noise = readKeyDouble(BAYESIAN_CONFIG, "noise");

    parameters.load_save_flag = 2;
    parameters.save_filename = optimisation_save_path.string();

    parameters.verbose_level = readKeyInt(BAYESIAN_CONFIG, "print_verbose");
    parameters.log_filename = optimisation_log_path.string();

    QuantifiedConfig best_pattern = generalOptimiser(seeds, threads, desired_pattern, weights, initial_config,
                                                     parameters, 3);
    std::vector<QuantifiedConfig> best_seed = best_pattern.findBestSeeds(readKeyInt(DISAGREEMENT_CONFIG, "final_seeds"), threads);

    exportPatternToDirectory(best_seed.getFilledPattern(), pattern_path);
    best_seed.getConfig().exportConfig(pattern_path);
    best_seed.getConfig().printConfig();
    best_seed.printDisagreement();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Execution time " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " s."
              << std::endl;
}


void optimisePattern(const fs::path &pattern_path) {
    optimisePattern(pattern_path, readKeyInt(DISAGREEMENT_CONFIG, "seeds"), readKeyInt(DISAGREEMENT_CONFIG, "threads"));
}


void fillPattern(const fs::path &pattern_path, const fs::path &config_path) {
    std::cout << "\n\nCurrent directory: " << pattern_path << std::endl;

    FillingConfig best_config(config_path);
    DesiredPattern desired_pattern = openPatternFromDirectory(pattern_path);
    DisagreementWeights default_weights(10, 2, 8, 2, 100, 2, 10, 2);

    QuantifiedConfig pattern(desired_pattern, best_config, default_weights);
    pattern.evaluate();

    best_config.printConfig();
    pattern.printDisagreement();

    exportPatternToDirectory(pattern.getFilledPattern(), pattern_path);
    pattern.getConfig().exportConfig(pattern_path);
}

void recalculateBestConfig(const fs::path &pattern_path) {
    fs::path config_path = pattern_path / "results" / "best_config.txt";
    fillPattern(pattern_path, config_path);
}