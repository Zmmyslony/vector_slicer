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
// Created by Michał Zmyślony on 10/01/2022.
//
#include <omp.h>
#include <iostream>
#include <utility>
#include <cfloat>
#include <vector>
#include <boost/filesystem.hpp>

#include "FillingOptimization.h"
#include "ConfigGeneration.h"
#include "../importing_and_exporting/OpenFiles.h"

#include "../pattern/FillingPatterns.h"
#include "../pattern/IndexedPath.h"
#include "../importing_and_exporting/Exporting.h"
#include "../auxiliary/ProgressBar.h"

#include "../../ExecutionConfig.h"

namespace fs = boost::filesystem;

FillingOptimization::FillingOptimization(DesiredPattern desired_pattern, const FillingConfig &initial_config, int min_seed,
                                         int max_seed, int threads) :
        desired_pattern(std::move(desired_pattern)),
        best_config(initial_config),
        min_seed(min_seed),
        max_seed(max_seed),
        threads(threads) {}


FillingOptimization::FillingOptimization(DesiredPattern desired_pattern, const FillingConfig &initial_config) :
        FillingOptimization(std::move(desired_pattern), initial_config, 0, 1, 1) {}


ConfigDisagreement selectBestFilling(const std::vector<ConfigDisagreement> &filled_patterns) {
    double lowest_disagreement = DBL_MAX;
    ConfigDisagreement best_fill = filled_patterns[0];

    for (auto &optimized_config: filled_patterns) {
        double current_disagreement = optimized_config.getDisagreement();
        if (current_disagreement < lowest_disagreement) {
            lowest_disagreement = current_disagreement;
            best_fill = optimized_config;
        }
    }
    return best_fill;
}


bool areConfigsTheSame(const ConfigDisagreement &first_config, const ConfigDisagreement &second_config) {
    FillingConfig first = first_config.getConfig();
    FillingConfig second = second_config.getConfig();
    return areFillingConfigsTheSame(first, second);
}


std::vector<std::vector<ConfigDisagreement>>
groupConfigsWithDifferentSeeds(std::vector<ConfigDisagreement> filled_patterns) {
    std::vector<std::vector<ConfigDisagreement>> grouped_configs;
    while (!filled_patterns.empty()) {
        ConfigDisagreement current_config = filled_patterns.back();
        filled_patterns.pop_back();
        bool was_config_added = false;
        for (auto &config_group: grouped_configs) {
            if (areConfigsTheSame(current_config, config_group[0])) {
                config_group.push_back(current_config);
                was_config_added = true;
            }
        }

        if (!was_config_added) {
            std::vector<ConfigDisagreement> new_group = {current_config};
            grouped_configs.push_back(new_group);
        }
    }
    return grouped_configs;
}


struct LessThanKey {
    inline bool operator() (const ConfigDisagreement &disagreement, const ConfigDisagreement &other_disagreement) {
        return (disagreement.getDisagreement() < other_disagreement.getDisagreement());
    }
};


std::vector<ConfigDisagreement> sortDisagreements(std::vector<ConfigDisagreement> config_group) {
    std::sort(config_group.begin(), config_group.end(), LessThanKey());
    return config_group;
}

std::vector<std::vector<ConfigDisagreement>> sortDisagreements(const std::vector<std::vector<ConfigDisagreement>> &config_groups) {
    std::vector<std::vector<ConfigDisagreement>> sorted_configs;
    sorted_configs.reserve(config_groups.size());
    for (auto &config_group: config_groups) {
        sorted_configs.emplace_back(sortDisagreements(config_group));
    }
    return sorted_configs;
}


double getMedianDisagreement(const std::vector<ConfigDisagreement> &config_group) {
    std::vector<double> disagreement_vector;
    disagreement_vector.reserve(config_group.size());
    for (auto &config: config_group) {
        disagreement_vector.emplace_back(config.getDisagreement());
    }
    std::sort(disagreement_vector.begin(), disagreement_vector.end());
    return disagreement_vector[disagreement_vector.size() * 1 / 4 - 1];
}


std::vector<double> getMedianDisagreement(const std::vector<std::vector<ConfigDisagreement>> &groups_of_configs) {
    std::vector<double> median_disagreement;
    median_disagreement.reserve(groups_of_configs.size());
    for (auto &config_group: groups_of_configs) {
        median_disagreement.emplace_back(getMedianDisagreement(config_group));
    }
    return median_disagreement;
}


ConfigDisagreement selectBestFillingMedian(const std::vector<ConfigDisagreement> &filled_patterns) {
    std::vector<std::vector<ConfigDisagreement>> config_groups = groupConfigsWithDifferentSeeds(filled_patterns);
    std::vector<std::vector<ConfigDisagreement>> sorted_config_groups = sortDisagreements(config_groups);
    std::vector<double> median_disagreements = getMedianDisagreement(sorted_config_groups);

    int min_element_index =
            std::min_element(median_disagreements.begin(), median_disagreements.end()) - median_disagreements.begin();

    std::vector<ConfigDisagreement> best_group = sorted_config_groups[min_element_index];
    return best_group[best_group.size() * 1 / 4 - 1];
}


std::vector<ConfigDisagreement> configDisagreementFromConfigs(const std::vector<FillingConfig> &configs) {
    std::vector<ConfigDisagreement> filling_configs;
    filling_configs.reserve(configs.size());
    for (auto &config: configs) {
        filling_configs.emplace_back(config);
    }
    return filling_configs;
}


std::vector<ConfigDisagreement>
calculateFills(const DesiredPattern &desired_pattern, std::vector<ConfigDisagreement> filling_configs,
               int threads) {
    omp_set_num_threads(threads);
    int progress = 0;
    int total_number_of_steps = filling_configs.size();
#pragma omp parallel for
    for (int i = 0; i < filling_configs.size(); i++) {
//        showProgress(progress, total_number_of_steps);
        filling_configs[i].fillWithPatterns(desired_pattern);
        progress++;
        showProgress(progress, total_number_of_steps);
    }
    std::cout << "\r";
    return filling_configs;
}


std::vector<ConfigDisagreement>
fillConfigs(const DesiredPattern &desired_pattern, const std::vector<FillingConfig> &list_of_configs,
            int threads) {
    std::vector<ConfigDisagreement> configs_to_test = configDisagreementFromConfigs(list_of_configs);
    std::vector<ConfigDisagreement> filled_configs = calculateFills(desired_pattern, configs_to_test, threads);
    return filled_configs;
}


FillingConfig
selectBestConfigMethod(const DesiredPattern &desired_pattern, std::vector<FillingConfig> &configs, int threads,
                       ConfigDisagreement (*selection_method)(const std::vector<ConfigDisagreement> &)) {
    std::vector<ConfigDisagreement> filled_configs = fillConfigs(desired_pattern, configs, threads);
    ConfigDisagreement best_fill = selection_method(filled_configs);
    printf("\tDisagreement %.3f.", best_fill.getDisagreement());
    std::cout << std::endl;
    return best_fill.getConfig();
}


FillingConfig
selectBestConfigMedian(const DesiredPattern &desired_pattern, std::vector<FillingConfig> &configs, int threads) {
    return selectBestConfigMethod(desired_pattern, configs, threads, &selectBestFillingMedian);
}


FillingConfig
selectBestConfigSingle(const DesiredPattern &desired_pattern, std::vector<FillingConfig> &configs, int threads) {
    return selectBestConfigMethod(desired_pattern, configs, threads, &selectBestFilling);
}


FillingConfig
findBestFillingForSeeds(const DesiredPattern &desired_pattern, const std::vector<FillingConfig> &config_list,
                        int min_seed, int max_seed, int threads, const std::string &type, configOptions optimized_option,
                        const std::string &initial_value) {
    std::cout << "Optimizing over " << type << ". " << std::endl;

    std::vector<FillingConfig> configs_to_test = iterateOverSeeds(desired_pattern, config_list, min_seed, max_seed);
    FillingConfig best_config = selectBestConfigMedian(desired_pattern, configs_to_test, threads);

    std::cout << "\tInitial value: " << stod(initial_value) << ". Optimized value: "
              << stod(best_config.getConfigOption(optimized_option)) << std::endl << std::endl;

    return best_config;
}


void FillingOptimization::optimizeOption(double delta, int steps, configOptions option, const std::string &name) {
    std::vector<FillingConfig> config_list = iterateOverOption(desired_pattern, best_config, delta, steps, option);
    std::string initial_value = best_config.getConfigOption(option);

    best_config = findBestFillingForSeeds(desired_pattern, config_list, min_seed, max_seed, threads,
                                          name, option, initial_value);
}


void FillingOptimization::optimizeRepulsion(double delta, int steps) {
    optimizeOption(delta, steps, Repulsion, "repulsion");
}

void FillingOptimization::optimizeCollisionRadius(double delta, int steps) {
    optimizeOption(delta, steps, CollisionRadius, "collision radius");
}

void FillingOptimization::optimizeStartingDistance(double delta, int steps) {
    if (steps > delta) {
        steps = (int) delta;
    }
    optimizeOption(delta, steps, StartingPointSeparation, "starting point separation");
}

const FillingConfig &FillingOptimization::getBestConfig() const {
    return best_config;
}


void FillingOptimization::optimizeSeeds(int multiplier) {
    std::cout << "Finding the best seed." << std::endl;
    std::vector<FillingConfig> configs = iterateOverSeeds(desired_pattern, best_config, min_seed, max_seed * multiplier);
    best_config = selectBestConfigSingle(desired_pattern, configs, threads);
    std::cout << std::endl;
}


ConfigDisagreement configurationOptimizer(const DesiredPattern &desired_pattern, FillingConfig initial_config,
                                          int min_seed, int max_seed, int threads) {
    FillingOptimization optimization(desired_pattern, initial_config, min_seed, max_seed, threads);

    optimization.optimizeStartingDistance(8, 8);
    optimization.optimizeRepulsion(0.5, 4);
    optimization.optimizeCollisionRadius(4, 4);

    optimization.optimizeStartingDistance(4, 4);
    optimization.optimizeRepulsion(0.25, 4);
    optimization.optimizeCollisionRadius(2, 4);

    optimization.optimizeStartingDistance(2, 2);
    optimization.optimizeRepulsion(0.125, 4);
    optimization.optimizeCollisionRadius(1, 4);

    optimization.optimizeSeeds(10);

    ConfigDisagreement best_fill(optimization.getBestConfig());
    best_fill.fillWithPatterns(desired_pattern);
    FillingConfig best_config = optimization.getBestConfig();
    best_config.printConfig();
    return best_fill;
}


ConfigDisagreement
seedOptimizer(const DesiredPattern &desired_pattern, FillingConfig initial_config, int min_seed, int max_seed,
              int threads) {
    FillingOptimization optimization(desired_pattern, initial_config, min_seed, max_seed, threads);

    optimization.optimizeSeeds(1);

    ConfigDisagreement best_fill(optimization.getBestConfig());
    best_fill.fillWithPatterns(desired_pattern);
    FillingConfig best_config = optimization.getBestConfig();
    best_config.printConfig();
    return best_fill;
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


void generalFinder(const fs::path &pattern_path, int min_seed, int max_seed, int threads,
                   ConfigDisagreement (*optimizer)(const DesiredPattern &, FillingConfig, int, int, int)) {
    time_t start_time = clock();
    std::cout << "\n\nCurrent directory: " << pattern_path << std::endl;
    DesiredPattern desired_pattern = openPatternFromDirectory(pattern_path);
    fs::path config_path = pattern_path / "config.txt";
    FillingConfig initial_config(config_path);

    ConfigDisagreement best_filling = optimizer(desired_pattern, initial_config, min_seed, max_seed, threads);
    FillingConfig best_config = best_filling.getConfig();
    best_config.exportConfig(pattern_path);

    exportPatternToDirectory(best_filling.getPattern(desired_pattern), pattern_path);
    printf("Multi-thread execution time %.2f", (double) (clock() - start_time) / CLOCKS_PER_SEC);
}


void findBestConfig(const fs::path &pattern_path, int min_seed, int max_seed, int threads) {
    generalFinder(pattern_path, min_seed, max_seed, threads, &configurationOptimizer);
}


void findBestSeed(const fs::path &pattern_path, int min_seed, int max_seed, int threads) {
    generalFinder(pattern_path, min_seed, max_seed, threads, &seedOptimizer);
}


void recalculateBestConfig(const fs::path &pattern_path) {
    time_t start_time = clock();
    std::cout << "\n\nCurrent directory: " << pattern_path << std::endl;
    DesiredPattern desired_pattern = openPatternFromDirectory(pattern_path);
    fs::path config_path = pattern_path / "results" /" best_config.txt";
    FillingConfig config(config_path);

    ConfigDisagreement filling(config);
    filling.fillWithPatterns(desired_pattern);

    exportPatternToDirectory(filling.getPattern(desired_pattern), pattern_path);
    printf("Execution time %.2f", (double) (clock() - start_time) / CLOCKS_PER_SEC);
}