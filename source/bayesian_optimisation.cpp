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
#include <iostream>
#include <string>
#include <iomanip>

#include "bayesian_optimisation.h"
#include "vector_slicer_config.h"
#include "pattern/director_indexed_path.h"
#include "pattern/vector_sorted_paths.h"
#include "pattern/importing_and_exporting/open_files.h"
#include "pattern/importing_and_exporting/exporting.h"
#include "pattern/auxiliary/progress_bar.h"
#include "pattern/simulation/configuration_reading.h"
#include "dataset.hpp"

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
        disagreement_percentile(readKeyDouble(DISAGREEMENT_CONFIG, "agreement_percentile")),
        is_collision_radius_optimised(readKeyBool(BAYESIAN_CONFIG, "is_collision_radius_optimised")),
        is_repulsion_angle_optimised(readKeyBool(BAYESIAN_CONFIG, "is_repulsion_angle_optimised")),
        is_repulsion_magnitude_optimised(readKeyBool(BAYESIAN_CONFIG, "is_repulsion_magnitude_optimised")),
        is_starting_point_separation_optimised(
                readKeyBool(BAYESIAN_CONFIG, "is_starting_point_separation_optimised")) {
}


double BayesianOptimisation::evaluateSample(const vectord &x_in) {
    if (x_in.size() != dims) {
        std::cerr << "ERROR: This only works for " << dims << "D inputs." << std::endl
                  << "ERROR: Using only first " << dims << " components." << std::endl;
    }
    problem = QuantifiedConfig(problem, x_in);
    double disagreement = problem.getDisagreement(seeds, threads, is_disagreement_details_printed,
                                                  disagreement_percentile);

    return disagreement;
}

void
BayesianOptimisation::showProgress(int current_step, int max_step, int steps_from_improvement, int steps_threshold,
                                   int step_offset) {
    double min_value = getValueAtMinimum();
    vectord best_configuration = bayesopt::ContinuousModel::remapPoint(getData()->getPointAtMinimum());
    vecd vector_configuration(best_configuration.begin(), best_configuration.end());

    std::stringstream suffix_stream;
    suffix_stream << std::setprecision(3) << "Minimal disagreement: " << min_value << ", at:";
    if (is_collision_radius_optimised) {
        suffix_stream << " ColRad " << vector_configuration.back() << ";";
        vector_configuration.pop_back();
    }
    if (is_starting_point_separation_optimised) {
        suffix_stream << " Sep " << vector_configuration.back() << ";";
        vector_configuration.pop_back();
    }
    if (is_repulsion_magnitude_optimised) {
        suffix_stream << " Rep " << vector_configuration.back() << ";";
        vector_configuration.pop_back();
    }
    if (is_repulsion_angle_optimised) {
        suffix_stream << " RepAng " << vector_configuration.back() << ";";
        vector_configuration.pop_back();
    }
    suffix_stream << ". Steps since improvement: " << steps_from_improvement;
    if (steps_threshold > 0) {
        suffix_stream << "/" << steps_threshold;
    }

    std::string suffix = suffix_stream.str();
    current_step += step_offset;
    if (max_step > 0) {
        max_step += step_offset;
    }
    showProgressBase(current_step, max_step, steps_from_improvement, steps_threshold, begin,
                     std::chrono::steady_clock::now(), suffix);
}

void BayesianOptimisation::optimizeControlled(vectord &x_out, int max_steps, int max_constant_steps) {
    initializeOptimization();
    if (max_steps <= 0 && max_constant_steps <= 0) {
        throw std::runtime_error(
                "Optimisation incorrectly set up. At least one of number_of_iterations or "
                "number_of_improvement_iterations needs to be greater than zero.");
    }
    int steps_since_improvement = 0;
    int i = 0;
    while (true) {
        double minimal_disagreement = getValueAtMinimum();
        stepOptimization();
        vectord best_configuration = bayesopt::ContinuousModel::remapPoint(getData()->getPointAtMinimum());
        double current_disagreement = getData()->getLastSampleY();
        if (current_disagreement < minimal_disagreement) {
            steps_since_improvement = 0;
        }
        showProgress(i, max_steps, steps_since_improvement,
                     max_constant_steps, mParameters.n_init_samples);
        steps_since_improvement++;

        if (max_constant_steps > 0 && steps_since_improvement > max_constant_steps) {
            std::cout
                    << "\rSteps since last improvement of the disagreement exceeded after " << mCurrentIter
                    << " steps. Finishing optimisation."
                    << std::endl;
            break;
        } else if (current_disagreement == 0) {
            std::cout << "\rIdeal filling found after " << mCurrentIter << " steps. Finishing optimisation."
                      << std::endl;
            break;
        }
        i++;
        if (max_steps > 0 && i >= max_steps) {
            break;
        }
    }
    x_out = getFinalResult();
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

fs::path
createPathWithExtension(const std::string &directory, const std::string &filename, const std::string &extension) {
    fs::path directory_path = directory;
    createDirectory(directory_path);
    fs::path joined_path = directory_path / filename;
    joined_path.replace_extension(extension);
    return joined_path;
}

fs::path createTxtPath(const std::string &directory, const std::string &filename) {
    return createPathWithExtension(directory, filename, "txt");
}

fs::path createCsvPath(const std::string &directory, const std::string &filename) {
    return createPathWithExtension(directory, filename, "csv");
}


void exportPatterns(const std::vector<QuantifiedConfig> &patterns, const fs::path &pattern_path) {
    fs::path results_directory = pattern_path / "results";

    fs::path output_directory = pattern_path.parent_path().parent_path() / "output";
    createDirectory(output_directory);
    std::string pattern_name = pattern_path.stem().string();

    fs::path generated_paths_directory = createCsvPath(PATHS_EXPORT_PATH, pattern_name);
    fs::path matrices_directory = createCsvPath(FILLED_MATRIX_EXPORT_PATH, pattern_name);
    fs::path best_config_directory = createTxtPath(CONFIG_EXPORT_PATH, pattern_name);

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


    double print_radius = pattern.getConfig().getPrintRadius();
    vecd lower_bound_vector;
    vecd upper_bound_vector;

    if (readKeyBool(BAYESIAN_CONFIG, "is_collision_radius_optimised")) {
        lower_bound_vector.emplace_back(0);
        upper_bound_vector.emplace_back(print_radius + 1);
    }
    if (readKeyBool(BAYESIAN_CONFIG, "is_starting_point_separation_optimised")) {
        lower_bound_vector.emplace_back(print_radius * 1.5);
        upper_bound_vector.emplace_back(print_radius * 2.5);
    }
    if (readKeyBool(BAYESIAN_CONFIG, "is_repulsion_magnitude_optimised")) {
        lower_bound_vector.emplace_back(0);
        upper_bound_vector.emplace_back(4);
    }
    if (readKeyBool(BAYESIAN_CONFIG, "is_repulsion_angle_optimised")) {
        lower_bound_vector.emplace_back(0);
        upper_bound_vector.emplace_back(M_PI / 2);
    }

    std::reverse(lower_bound_vector.begin(), lower_bound_vector.end());
    std::reverse(upper_bound_vector.begin(), upper_bound_vector.end());
    vectord best_config(dims);
    vectord lower_bound(dims);
    vectord upper_bound(dims);
    for (int i = 0; i < dims; i++) {
        lower_bound[i] = lower_bound_vector[i];
        upper_bound[i] = upper_bound_vector[i];
    }

    pattern_optimisation.setBoundingBox(lower_bound, upper_bound);
    int max_iterations = readKeyInt(BAYESIAN_CONFIG, "number_of_iterations");
    int max_iterations_without_improvement = readKeyInt(BAYESIAN_CONFIG, "number_of_improvement_iterations");

    pattern_optimisation.optimizeControlled(best_config, max_iterations, max_iterations_without_improvement);

    return {pattern, best_config};
}


void optimisePattern(const fs::path &pattern_path, int seeds, int threads) {
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    std::cout << "\n\nCurrent directory: " << pattern_path << std::endl;
    std::string pattern_name = pattern_path.stem().string();
    fs::path initial_config_path = pattern_path / "config.txt";

    createDirectory(pattern_path.parent_path().parent_path() / "output");
    createDirectory(LOGS_EXPORT_PATH);
    createDirectory(OPTIMISATION_EXPORT_PATH);
    fs::path optimisation_log_path = createTxtPath(LOGS_EXPORT_PATH, pattern_name);
    fs::path optimisation_save_path = createTxtPath(OPTIMISATION_EXPORT_PATH, pattern_name);

    FillingConfig initial_config(initial_config_path);
    bool is_splay_filling_enabled = initial_config.getInitialSeedingMethod() == Splay;
    DesiredPattern desired_pattern = openPatternFromDirectory(pattern_path, is_splay_filling_enabled);

    DisagreementWeights weights(DISAGREEMENT_FUNCTION_CONFIG);
    fs::path local_disagreement_path = pattern_path / "disagreement_function.cfg";
    if (fs::exists(local_disagreement_path)) {
        std::cout << "Local disagreement weights found." << std::endl;
        weights = DisagreementWeights(local_disagreement_path);
    }

    bayesopt::Parameters parameters;
    parameters.random_seed = 0;
    parameters.l_type = L_MCMC;
    parameters.n_iter_relearn = readKeyInt(BAYESIAN_CONFIG, "iterations_between_relearning");
    parameters.noise = readKeyDouble(BAYESIAN_CONFIG, "noise");
    parameters.n_inner_iterations = 1000;

    parameters.load_save_flag = 2;
    parameters.save_filename = optimisation_save_path.string();

    parameters.verbose_level = readKeyInt(BAYESIAN_CONFIG, "print_verbose");
    parameters.log_filename = optimisation_log_path.string();

    int dims = (int) readKeyBool(BAYESIAN_CONFIG, "is_collision_radius_optimised") +
               (int) readKeyBool(BAYESIAN_CONFIG, "is_repulsion_angle_optimised") +
               (int) readKeyBool(BAYESIAN_CONFIG, "is_repulsion_magnitude_optimised") +
               (int) readKeyBool(BAYESIAN_CONFIG, "is_starting_point_separation_optimised");
    if (dims == 0) {
        throw std::runtime_error(
                "No parameter was chosen for optimisation. Please enable at least one of them in bayesian_configuration.cfg");
    }
    QuantifiedConfig best_pattern = generalOptimiser(seeds, threads, desired_pattern, weights, initial_config,
                                                     parameters, dims);
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
    std::vector<std::vector<double>> disagreement_grid = best_fills[0].localDisagreementGrid();
    fs::path disagreement_grid_path = pattern_path / "disagreement_grid";
    exportVectorTableToFile(disagreement_grid, disagreement_grid_path);
}


void optimisePattern(const fs::path &pattern_path) {
    optimisePattern(pattern_path, readKeyInt(DISAGREEMENT_CONFIG, "seeds"), readKeyInt(DISAGREEMENT_CONFIG, "threads"));
}


void fillPattern(const fs::path &pattern_path, const fs::path &config_path) {
    std::cout << "\n\nCurrent directory: " << pattern_path << std::endl;

    DesiredPattern desired_pattern = openPatternFromDirectory(pattern_path, false);
    DisagreementWeights weights(DISAGREEMENT_FUNCTION_CONFIG);

    std::vector<FillingConfig> best_config = readMultiSeedConfig(config_path);
    std::vector<QuantifiedConfig> filled_configs;
    for (int i = 0; i < 10; i++) {
        filled_configs.emplace_back(QuantifiedConfig(desired_pattern, best_config[i], weights));
    }

    int threads = readKeyInt(DISAGREEMENT_CONFIG, "threads");
    omp_set_num_threads(threads);
#pragma omp parallel for
    for (int i = 0; i < filled_configs.size(); i++) {
        filled_configs[i].evaluate();
    }
    filled_configs[0].getConfig().printConfig();
    filled_configs[0].printDisagreement();
    exportPatterns(filled_configs, pattern_path);
    std::cout << "Pattern filled." << std::endl;
}

void recalculateBestConfig(const fs::path &pattern_path) {
    std::string pattern_name = pattern_path.stem().string() + ".txt";
    fs::path config_path = pattern_path.parent_path().parent_path() / "output" / "best_configs" / pattern_name;
    config_path.replace_extension("txt");
    fillPattern(pattern_path, config_path);
}