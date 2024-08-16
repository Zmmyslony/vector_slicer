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
#include <chrono>

#include "bayesian_optimisation.h"
#include "vector_slicer_config.h"
#include "pattern/path_sorting/nearest_neighbour.h"
#include "pattern/path_sorting/seed_line.h"
#include "pattern/importing_and_exporting/open_files.h"
#include "pattern/importing_and_exporting/exporting.h"
#include "pattern/auxiliary/progress_bar.h"
#include "pattern/simulation/configuration_reading.h"
#include "dataset.hpp"

namespace fs = boost::filesystem;

using vald = std::valarray<double>;
using vali = std::valarray<int>;

using pattern = std::vector<std::vector<vali>>;


BayesianOptimisation::BayesianOptimisation(QuantifiedConfig problem, bayesopt::Parameters parameters, int dims) :
        ContinuousModel(dims, std::move(parameters)),
        dims(dims),
        problem(std::move(problem)),
        threads(problem.getThreads()),
        seeds(problem.getOptimisationSeeds()),
        begin(std::chrono::steady_clock::now()),
        is_disagreement_details_printed(problem.isDisagreementDetailsPrinted()),
        disagreement_percentile(problem.getAgreementPercentile()),
        is_collision_radius_optimised(problem.isCollisionRadiusOptimised()),
        is_repulsion_angle_optimised(problem.isRepulsionAngleOptimised()),
        is_repulsion_magnitude_optimised(problem.isRepulsionMagnitudeOptimised()),
        is_starting_point_separation_optimised(problem.isStartingPointSeparationOptimised()) {
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
    std::cout << "Evaluating the pattern for initial samples." << std::endl;
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
                     max_constant_steps, (int)mParameters.n_init_samples);
        x_out = bayesopt::ContinuousModel::remapPoint(getData()->getPointAtMinimum());
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

void exportConfigList(const std::vector<QuantifiedConfig> &configs, const fs::path& path, int number_of_configs) {
    std::vector<FillingConfig> filling_configs;

    for (int i = 0; i < number_of_configs; i++) {
        filling_configs.push_back(configs[i].getConfig());
    }
    exportConfigList(filling_configs, path);
}

fs::path
createPathWithExtension(const std::string &directory, const std::string &filename, const std::string &extension) {
    fs::path directory_path = directory;
    createDirectory(directory_path);
    fs::path joined_path = directory_path / filename;
    joined_path += extension;
    return joined_path;
}

fs::path createTxtPath(const std::string &directory, const std::string &filename) {
    return createPathWithExtension(directory, filename, ".txt");
}

fs::path createCsvPath(const std::string &directory, const std::string &filename) {
    return createPathWithExtension(directory, filename, ".csv");
}

/**
 * Sorts paths from a pattern, using either vector or director sorting using either nearest neighbour or seed-line
 * ordering.
 * @param pattern
 * @param start coordinates where from the sorting should start from. Is overwritten by last position of the last path in sequence.
 * @return
 */
std::vector<Path> sort_paths(const FilledPattern &pattern, vali &start) {
    int sorting_method = pattern.desired_pattern.get().getSortingMethod();
    std::vector<Path> sorted_paths;
    switch (sorting_method) {
        case SORT_NEAREST_NEIGHBOUR:
            sorted_paths = nearestNeighbourSort(pattern, start);
            break;
        case SORT_SEED_LINE:
            sorted_paths = seedLineSort(pattern, start);
            break;
        default:
            throw std::runtime_error("ERROR: Unrecognised path sorting method.");
    }

    // Update for use in next layer.
    start = sorted_paths.back().endPoint();
    return sorted_paths;
}

/// Extracts coordinate sequences from path sequence.
std::vector<std::vector<vali>> extract_coordinates(const std::vector<Path> &paths) {
    std::vector<std::vector<vali>> position_sequences;
    position_sequences.reserve(paths.size());
    for (auto &path: paths) {
        position_sequences.emplace_back(path.getPositionSequence());
    }
    return position_sequences;
}

/// Extracts overlap sequence from path sequence.
std::vector<std::vector<double>> extract_overlap(const std::vector<Path> &paths) {
    std::vector<std::vector<double>> overlap_sequence;
    overlap_sequence.reserve(paths.size());
    for (auto &path: paths) {
        overlap_sequence.emplace_back(path.getOverlap());
    }
    return overlap_sequence;
}


void exportPatterns(const std::vector<QuantifiedConfig> &patterns, const fs::path &pattern_path,
                    const Simulation &simulation) {
    fs::path results_directory = pattern_path / "results";

    fs::path output_directory = OUTPUT_PATH;
    createDirectory(output_directory);
    std::string pattern_name = pattern_path.filename().string();

    fs::path generated_paths_directory = createCsvPath(PATHS_EXPORT_PATH, pattern_name);
    fs::path matrices_directory = createCsvPath(FILLED_MATRIX_EXPORT_PATH, pattern_name);
    fs::path best_config_directory = createTxtPath(CONFIG_EXPORT_PATH, pattern_name);
    fs::path overlap_directory = createCsvPath(OVERLAP_EXPORT_PATH, pattern_name);
    fs::path seed_directory = createCsvPath(SEED_EXPORT_PATH, pattern_name);
    fs::path bucketed_disagreement = createCsvPath(DISAGREEMENT_BUCKETS_PATH, pattern_name);

    std::vector<pattern> sorted_patterns;
    std::vector<std::vector<std::vector<double>>> sorted_overlaps;
    double print_diameter = 0;
    int number_of_layers = patterns[0].getNumberOfLayers();
    vali starting_coordinates = {0, 0};
    for (int i = 0; i < number_of_layers; i++) {
        FilledPattern pattern = patterns[i].getFilledPattern();
        pattern.updatePathsOverlap();
        std::vector<Path> sorted_paths = sort_paths(pattern, starting_coordinates);

        sorted_patterns.emplace_back(extract_coordinates(sorted_paths));
        sorted_overlaps.emplace_back(extract_overlap(sorted_paths));
        print_diameter = pattern.getPrintRadius() * 2;
    }

    exportConfigList(patterns, best_config_directory, number_of_layers);
    patterns[0].getFilledPattern().exportFilledMatrix(matrices_directory);
    std::vector<coord> seeds = patterns[0].getFilledPattern().getSeedCoordinates();
    exportCoordVector(seeds, seed_directory);
    exportPathSequence(sorted_patterns, generated_paths_directory, pattern_name, print_diameter, simulation);
    exportOverlap(sorted_overlaps, overlap_directory, pattern_name, print_diameter, simulation);
    exportRowToFile(patterns[0].getDirectorDisagreementDistribution(), bucketed_disagreement);
}

QuantifiedConfig generalOptimiser(const DesiredPattern &desired_pattern,
                                  FillingConfig filling_config,
                                  Simulation &simulation,
                                  bayesopt::Parameters optimisation_parameters, int dims) {

    QuantifiedConfig pattern(desired_pattern, filling_config, simulation);
    BayesianOptimisation pattern_optimisation(pattern, std::move(optimisation_parameters), dims);

    double print_radius = filling_config.getPrintRadius();
    vecd lower_bound_vector;
    vecd upper_bound_vector;
    vecd best_config_vector;

    if (pattern.isCollisionRadiusOptimised()) {
        lower_bound_vector.emplace_back(0);
        best_config_vector.emplace_back(filling_config.getTerminationRadius());
        upper_bound_vector.emplace_back(print_radius + 1);
    }
    if (pattern.isStartingPointSeparationOptimised()) {
        lower_bound_vector.emplace_back(print_radius * 1.6);
        best_config_vector.emplace_back(filling_config.getSeedSpacing());
        upper_bound_vector.emplace_back(print_radius * 3);
    }
    if (pattern.isRepulsionMagnitudeOptimised()) {
        lower_bound_vector.emplace_back(0);
        best_config_vector.emplace_back(filling_config.getRepulsion());
        upper_bound_vector.emplace_back(4);
    }
    if (pattern.isRepulsionAngleOptimised()) {
        lower_bound_vector.emplace_back(0);
        best_config_vector.emplace_back(filling_config.getRepulsionAngle());
        upper_bound_vector.emplace_back(M_PI / 2);
    }

    std::reverse(lower_bound_vector.begin(), lower_bound_vector.end());
    std::reverse(upper_bound_vector.begin(), upper_bound_vector.end());
    std::reverse(best_config_vector.begin(), best_config_vector.end());
    vectord best_config(dims);
    vectord lower_bound(dims);
    vectord upper_bound(dims);
    for (int i = 0; i < dims; i++) {
        lower_bound[i] = lower_bound_vector[i];
        upper_bound[i] = upper_bound_vector[i];
        best_config[i] = best_config_vector[i];
    }

    pattern_optimisation.setBoundingBox(lower_bound, upper_bound);
    int max_iterations = pattern.getTotalIterations();
    int max_iterations_without_improvement = pattern.getImprovementIterations();
    try {
        pattern_optimisation.optimizeControlled(best_config, max_iterations, max_iterations_without_improvement);
    } catch (std::runtime_error &error) {
        if (error.what() == std::string("nlopt failure")) {
            std::cout << std::endl;
            std::cout << "WARNING: NLOPT failure during bayesian optimisation. Returned solution may not be optimal."
                      << std::endl;
            return {pattern, best_config};
        }
        else {
            throw error;
        }
    }
    return {pattern, best_config};
}


void optimisePattern(const fs::path &pattern_path, bool is_default_used) {
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    std::cout << "\n\nCurrent directory: " << pattern_path << std::endl;
    std::string pattern_name = pattern_path.filename().string();
    fs::path initial_config_path = pattern_path / "config.txt";

    createDirectory(pattern_path.parent_path().parent_path() / "output");
    createDirectory(LOGS_EXPORT_PATH);
    createDirectory(OPTIMISATION_EXPORT_PATH);
    fs::path optimisation_log_path = createTxtPath(LOGS_EXPORT_PATH, pattern_name);
    fs::path optimisation_save_path = createTxtPath(OPTIMISATION_EXPORT_PATH, pattern_name);

    Simulation simulation(pattern_path, is_default_used);
    FillingConfig initial_config(initial_config_path);
    bool is_splay_filling_enabled = initial_config.getInitialSeedingMethod() == Splay;
    DesiredPattern desired_pattern = openPatternFromDirectory(pattern_path, is_splay_filling_enabled,
                                                              simulation.getThreads(), simulation);

    QuantifiedConfig best_pattern(desired_pattern, initial_config, simulation);

    bayesopt::Parameters parameters;
    parameters.random_seed = 0;
    parameters.n_init_samples = 50;
    parameters.l_type = L_MCMC;
    parameters.force_jump = best_pattern.getRelearningIterations();
    parameters.n_iter_relearn = best_pattern.getRelearningIterations();
    parameters.noise = best_pattern.getNoise();
    parameters.n_inner_iterations = 1000;

    parameters.load_save_flag = 2;
    parameters.save_filename = optimisation_save_path.string();

    parameters.verbose_level = best_pattern.getPrintVerbose();
    parameters.log_filename = optimisation_log_path.string();

    int dims = (int) best_pattern.isRepulsionAngleOptimised() +
               (int) best_pattern.isRepulsionMagnitudeOptimised() +
               (int) best_pattern.isCollisionRadiusOptimised() +
               (int) best_pattern.isStartingPointSeparationOptimised();


    if (dims == 0) {
        std::cout << "No parameter was chosen for optimisation. Optimising only over seeds. \n"
                     "You can enable optimisation_parameters in bayesian_configuration.cfg" << std::endl;
    } else {
        best_pattern = generalOptimiser(desired_pattern, initial_config, simulation,
                                        parameters, dims);
    }

    std::vector<QuantifiedConfig> best_fills = best_pattern.findBestSeeds(
            best_pattern.getFinalSeeds(), best_pattern.getThreads());

    exportPatterns(best_fills, pattern_path, simulation);
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


void fillPattern(const fs::path &pattern_path, const fs::path &config_path) {
    std::cout << "\n\nCurrent directory: " << pattern_path << std::endl;

    Simulation simulation(pattern_path, true);
    if (!fs::exists(config_path)) { throw std::runtime_error("ERROR: Missing config path."); }
    std::vector<FillingConfig> best_config = readMultiSeedConfig(config_path);
    bool is_splay_filling_enabled = best_config[0].getInitialSeedingMethod() == Splay;
    DesiredPattern desired_pattern = openPatternFromDirectory(pattern_path, is_splay_filling_enabled,
                                                              simulation.getThreads(), simulation);
    std::vector<QuantifiedConfig> filled_configs;
    for (int i = 0; i < 10; i++) {
        filled_configs.emplace_back(desired_pattern, best_config[i], simulation);
    }

    int threads = simulation.getThreads();
    omp_set_num_threads(threads);
#pragma omp parallel for
    for (int i = 0; i < filled_configs.size(); i++) {
        filled_configs[i].evaluate();
    }
    filled_configs[0].getConfig().printConfig();
    filled_configs[0].printDisagreement();
    exportPatterns(filled_configs, pattern_path, simulation);
    std::cout << "Pattern filled." << std::endl;
}

void optimisePatternSeeds(const fs::path &pattern_path, const fs::path &config_path, int seeds) {
    std::cout << "\n\nCurrent directory: " << pattern_path << std::endl;

    Simulation simulation(pattern_path, true);
    if (!fs::exists(config_path)) { throw std::runtime_error("ERROR: Missing config path."); }
    std::vector<FillingConfig> best_config = readMultiSeedConfig(config_path);
    bool is_splay_filling_enabled = best_config[0].getInitialSeedingMethod() == Splay;
    DesiredPattern desired_pattern = openPatternFromDirectory(pattern_path, is_splay_filling_enabled,
                                                              simulation.getThreads(), simulation);
    QuantifiedConfig best_pattern(desired_pattern, best_config[0], simulation);

    std::vector<QuantifiedConfig> best_fills = best_pattern.findBestSeeds(seeds, best_pattern.getThreads());

    exportPatterns(best_fills, pattern_path, simulation);
    std::vector<FillingConfig> config_list;
    for (auto &filled_config: best_fills) {
        config_list.emplace_back(filled_config.getConfig());
    }

    best_fills[0].getConfig().printConfig();
    best_fills[0].printDisagreement();
}

void recalculateBestConfig(const fs::path &pattern_path) {
    std::string pattern_name = pattern_path.filename().string();
    fs::path config_path =
            pattern_path.parent_path().parent_path() / "output" / "best_configs" / (pattern_name + ".txt");
    fillPattern(pattern_path, config_path);
}