// Copyright (c) 2022-2023, Michał Zmyślony, mlz22@cam.ac.uk.
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
// Created by Michał Zmyślony on 21/11/2022.
//

#include <boost/filesystem/path.hpp>
#include <utility>
#include <chrono>
#include <omp.h>

#include "bayesian_optimisation.h"
#include "vector_slicer_config.h"
#include "pattern/director_indexed_path.h"
#include "pattern/vector_sorted_paths.h"
#include "pattern/importing_and_exporting/open_files.h"
#include "pattern/importing_and_exporting/exporting.h"
#include "pattern/auxiliary/progress_bar.h"
#include "pattern/auxiliary/configuration_reading.h"

namespace fs = boost::filesystem;

using vald = std::valarray<double>;
using vali = std::valarray<int>;

using pattern = std::vector<std::vector<vali>>;


BayesianOptimisation::BayesianOptimisation(QuantifiedConfig problem, int threads, int seeds,
                                           bayesopt::Parameters parameters, int dims) :
        ContinuousModel(dims, std::move(parameters)),
        dims(dims),
        problem(std::move(problem)),
        threads(threads),
        seeds(seeds),
        begin(std::chrono::steady_clock::now()),
        is_disagreement_details_printed(readKeyBool(DISAGREEMENT_CONFIG, "is_disagreement_details_printed")),
        disagreement_percentile(readKeyDouble(DISAGREEMENT_CONFIG, "agreement_percentile")){
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
    double disagreement = problem.getDisagreement(seeds, threads, is_disagreement_details_printed, disagreement_percentile);
    if (mCurrentIter > 0) {
        double minimal_disagreement = getValueAtMinimum();
        showProgress(mCurrentIter + mParameters.n_init_samples, mParameters.n_iterations + mParameters.n_init_samples,
                     begin, minimal_disagreement);
    }
    return disagreement;
}

void createDirectory(const fs::path &path) {
    if (!fs::exists(path)) {
        fs::create_directory(path);
    }
}

void exportConfigList(const std::vector<QuantifiedConfig> &configs, fs::path path, int number_of_configs) {
    std::vector<FillingConfig> filling_configs;

    for (int i = 0; i < number_of_configs; i++) {
        filling_configs.push_back(configs[i].getConfig());
    }
    exportConfigList(filling_configs, std::move(path));
}

fs::path createPathWithExtension(std::string directory, std::string filename, std::string extension) {
    fs::path directory_path = directory;
    createDirectory(directory_path);
    fs::path joined_path = directory_path / filename;
    joined_path.replace_extension(extension);
    return joined_path;
}

fs::path createTxtPath(std::string directory, std::string filename) {
    return createPathWithExtension(std::move(directory), std::move(filename), "txt");
}

fs::path createCsvPath(std::string directory, std::string filename) {
    return createPathWithExtension(std::move(directory), std::move(filename), "csv");
}


void exportPatterns(const std::vector<QuantifiedConfig> &patterns, const fs::path &pattern_path) {
    fs::path results_directory = pattern_path / "results";

    fs::path output_directory = pattern_path.parent_path().parent_path() / "output";
    createDirectory(output_directory);
    std::string pattern_name = pattern_path.stem().string();

    fs::path generated_paths_directory = createCsvPath(PATHS_EXPORT_PATH, pattern_name);
    fs::path matrices_directory = createCsvPath(FILLED_MATRIX_EXPORT_PATH, pattern_name);
    fs::path best_config_directory = createCsvPath(CONFIG_EXPORT_PATH, pattern_name);

    std::vector<pattern> sorted_patterns;
    double print_diameter = 0;
    int number_of_layers = readKeyInt(DISAGREEMENT_CONFIG, "number_of_layers");
    for (int i = 0; i < number_of_layers; i++) {
        FilledPattern pattern = patterns[i].getFilledPattern();
        if (pattern.desired_pattern.get().isVectorSorted()) {
            sorted_patterns.emplace_back(getVectorSortedPaths(pattern.getSequenceOfPaths(), {0, 0}));
        } else {
            sorted_patterns.emplace_back(getDirectorSortedPaths(pattern, 0));
        }
        print_diameter = pattern.getPrintRadius() * 2 + 1;
    }

    exportConfigList(patterns, best_config_directory, number_of_layers);
    patterns[0].getFilledPattern().exportFilledMatrix(matrices_directory);
    exportPathSequence(sorted_patterns, generated_paths_directory, pattern_name, print_diameter);
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
    std::string pattern_name = pattern_path.stem().string();
    fs::path config_path = createTxtPath(CONFIG_EXPORT_PATH, pattern_name);
    fs::path optimisation_log_path = createTxtPath(LOGS_EXPORT_PATH, pattern_name);
    fs::path optimisation_save_path = createTxtPath(OPTIMISATION_EXPORT_PATH, pattern_name);

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
    std::vector<QuantifiedConfig> best_fills = best_pattern.findBestSeeds(
            readKeyInt(DISAGREEMENT_CONFIG, "final_seeds"), threads);

    exportPatterns(best_fills, pattern_path);
    std::vector<FillingConfig> config_list;
    for (auto &filled_config: best_fills) {
        config_list.emplace_back(filled_config.getConfig());
    }

    best_fills[0].getConfig().printConfig();
    best_fills[0].printDisagreement();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Execution time " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " s."
              << std::endl;
}


void optimisePattern(const fs::path &pattern_path) {
    optimisePattern(pattern_path, readKeyInt(DISAGREEMENT_CONFIG, "seeds"), readKeyInt(DISAGREEMENT_CONFIG, "threads"));
}


void fillPattern(const fs::path &pattern_path, const fs::path &config_path) {
    std::cout << "\n\nCurrent directory: " << pattern_path << std::endl;

    DesiredPattern desired_pattern = openPatternFromDirectory(pattern_path);
    DisagreementWeights default_weights(10, 2, 8, 2, 100, 2, 10, 2);

    std::vector<FillingConfig> best_config = readMultiSeedConfig(config_path);
    std::vector<QuantifiedConfig> filled_configs;
    for (int i = 0; i < 10; i++) {
        filled_configs.emplace_back(QuantifiedConfig(desired_pattern, best_config[i], default_weights));
    }

    int threads = readKeyInt(DISAGREEMENT_CONFIG, "threads");
    omp_set_num_threads(threads);
#pragma omp parallel for
    for (int i = 0; i < filled_configs.size(); i++) {
        filled_configs[i].evaluate();
    }
    std::cout << filled_configs.size() << std::endl;
    exportPatterns(filled_configs, pattern_path);
}

void recalculateBestConfig(const fs::path &pattern_path) {
    std::string pattern_name = pattern_path.stem().string() + ".txt";
    fs::path config_path = pattern_path.parent_path().parent_path() / "output" / "best_configs" / pattern_name;
    fillPattern(pattern_path, config_path);
}