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
#include "OpenFiles.h"

#include "../pattern/FillingPatterns.h"
#include "../pattern/IndexedPath.h"
#include "../auxiliary/Exporting.h"
#include "../auxiliary/ProgressBar.h"

#include "../../ExecutionConfig.h"

namespace fs = boost::filesystem;

FillingOptimization::FillingOptimization(DesiredPattern desiredPattern, const FillingConfig &initialConfig, int minSeed,
                                         int maxSeed, int threads) :
        desiredPattern(std::move(desiredPattern)),
        bestConfig(initialConfig),
        minSeed(minSeed),
        maxSeed(maxSeed),
        threads(threads) {}


FillingOptimization::FillingOptimization(DesiredPattern desiredPattern, const FillingConfig &initialConfig) :
        FillingOptimization(std::move(desiredPattern), initialConfig, 0, 1, 1) {}


ConfigDisagreement selectBestFilling(const std::vector<ConfigDisagreement> &filledPatterns) {
    double lowestDisagreement = DBL_MAX;
    ConfigDisagreement bestFill = filledPatterns[0];

    for (auto &optimizedConfig: filledPatterns) {
        double currentDisagreement = optimizedConfig.getDisagreement();
        if (currentDisagreement < lowestDisagreement) {
            lowestDisagreement = currentDisagreement;
            bestFill = optimizedConfig;
        }
    }
    return bestFill;
}


bool areConfigsTheSame(const ConfigDisagreement &firstConfig, const ConfigDisagreement &secondConfig) {
    FillingConfig first = firstConfig.getConfig();
    FillingConfig second = secondConfig.getConfig();
    return areFillingConfigsTheSame(first, second);
}


std::vector<std::vector<ConfigDisagreement>>
groupConfigsWithDifferentSeeds(std::vector<ConfigDisagreement> filledPatterns) {
    std::vector<std::vector<ConfigDisagreement>> groupedConfigs;
    while (!filledPatterns.empty()) {
        ConfigDisagreement currentConfig = filledPatterns.back();
        filledPatterns.pop_back();
        bool wasConfigAdded = false;
        for (auto &configGroup: groupedConfigs) {
            if (areConfigsTheSame(currentConfig, configGroup[0])) {
                configGroup.push_back(currentConfig);
                wasConfigAdded = true;
            }
        }

        if (!wasConfigAdded) {
            std::vector<ConfigDisagreement> newGroup = {currentConfig};
            groupedConfigs.push_back(newGroup);
        }
    }
    return groupedConfigs;
}


struct lessThanKey {
    inline bool operator() (const ConfigDisagreement &disagreement, const ConfigDisagreement &otherDisagreement) {
        return (disagreement.getDisagreement() < otherDisagreement.getDisagreement());
    }
};


std::vector<ConfigDisagreement> sortDisagreements(std::vector<ConfigDisagreement> configGroup) {
    std::sort(configGroup.begin(), configGroup.end(), lessThanKey());
    return configGroup;
}

std::vector<std::vector<ConfigDisagreement>> sortDisagreements(const std::vector<std::vector<ConfigDisagreement>> &configGroups) {
    std::vector<std::vector<ConfigDisagreement>> sortedConfigs;
    sortedConfigs.reserve(configGroups.size());
    for (auto &configGroup: configGroups) {
        sortedConfigs.emplace_back(sortDisagreements(configGroup));
    }
    return sortedConfigs;
}


double getMedianDisagreement(const std::vector<ConfigDisagreement> &configGroup) {
    std::vector<double> disagreementVector;
    disagreementVector.reserve(configGroup.size());
    for (auto &config: configGroup) {
        disagreementVector.emplace_back(config.getDisagreement());
    }
    std::sort(disagreementVector.begin(), disagreementVector.end());
    return disagreementVector[disagreementVector.size() * 1 / 4 - 1];
}


std::vector<double> getMedianDisagreement(const std::vector<std::vector<ConfigDisagreement>> &groupsOfConfigs) {
    std::vector<double> medianDisagreement;
    medianDisagreement.reserve(groupsOfConfigs.size());
    for (auto &configGroup: groupsOfConfigs) {
        medianDisagreement.emplace_back(getMedianDisagreement(configGroup));
    }
    return medianDisagreement;
}


ConfigDisagreement selectBestFillingMedian(const std::vector<ConfigDisagreement> &filledPatterns) {
    std::vector<std::vector<ConfigDisagreement>> configGroups = groupConfigsWithDifferentSeeds(filledPatterns);
    std::vector<std::vector<ConfigDisagreement>> sortedConfigGroups = sortDisagreements(configGroups);
    std::vector<double> medianDisagreements = getMedianDisagreement(sortedConfigGroups);

    int minElementIndex =
            std::min_element(medianDisagreements.begin(), medianDisagreements.end()) - medianDisagreements.begin();

    std::vector<ConfigDisagreement> bestGroup = sortedConfigGroups[minElementIndex];
    return bestGroup[bestGroup.size() * 1 / 4 - 1];
}


std::vector<ConfigDisagreement> configDisagreementFromConfigs(const std::vector<FillingConfig> &configs) {
    std::vector<ConfigDisagreement> fillingConfigs;
    fillingConfigs.reserve(configs.size());
    for (auto &config: configs) {
        fillingConfigs.emplace_back(config);
    }
    return fillingConfigs;
}


std::vector<ConfigDisagreement>
calculateFills(const DesiredPattern &desiredPattern, std::vector<ConfigDisagreement> fillingConfigs,
               int threads) {
    omp_set_num_threads(threads);
    int progress = 0;
    int totalNumberOfSteps = fillingConfigs.size();
#pragma omp parallel for
    for (int i = 0; i < fillingConfigs.size(); i++) {
//        showProgress(progress, totalNumberOfSteps);
        fillingConfigs[i].fillWithPatterns(desiredPattern);
        progress++;
        showProgress(progress, totalNumberOfSteps);
    }
    std::cout << "\r";
    return fillingConfigs;
}


std::vector<ConfigDisagreement>
fillConfigs(const DesiredPattern &desiredPattern, const std::vector<FillingConfig> &listOfConfigs,
            int threads) {
    std::vector<ConfigDisagreement> configsToTest = configDisagreementFromConfigs(listOfConfigs);
    std::vector<ConfigDisagreement> filledConfigs = calculateFills(desiredPattern, configsToTest, threads);
    return filledConfigs;
}


FillingConfig
selectBestConfigMethod(const DesiredPattern &desiredPattern, std::vector<FillingConfig> &configs, int threads,
                       ConfigDisagreement (*selectionMethod)(const std::vector<ConfigDisagreement> &)) {
    std::vector<ConfigDisagreement> filledConfigs = fillConfigs(desiredPattern, configs, threads);
    ConfigDisagreement bestFill = selectionMethod(filledConfigs);
    printf("\tDisagreement %.3f.", bestFill.getDisagreement());
    std::cout << std::endl;
    return bestFill.getConfig();
}


FillingConfig
selectBestConfigMedian(const DesiredPattern &desiredPattern, std::vector<FillingConfig> &configs, int threads) {
    return selectBestConfigMethod(desiredPattern, configs, threads, &selectBestFillingMedian);
}


FillingConfig
selectBestConfigSingle(const DesiredPattern &desiredPattern, std::vector<FillingConfig> &configs, int threads) {
    return selectBestConfigMethod(desiredPattern, configs, threads, &selectBestFilling);
}


FillingConfig
findBestFillingForSeeds(const DesiredPattern &desiredPattern, const std::vector<FillingConfig> &configList,
                        int minSeed, int maxSeed, int threads, const std::string &type, ConfigOptions optimizedOption,
                        const std::string &initialValue) {
    std::cout << "Optimizing over " << type << ". " << std::endl;

    std::vector<FillingConfig> configsToTest = iterateOverSeeds(desiredPattern, configList, minSeed, maxSeed);
    FillingConfig bestConfig = selectBestConfigMedian(desiredPattern, configsToTest, threads);

    std::cout << "\tInitial value: " << stod(initialValue) << ". Optimized value: "
              << stod(bestConfig.getConfigOption(optimizedOption)) << std::endl << std::endl;

    return bestConfig;
}


void FillingOptimization::optimizeOption(double delta, int steps, ConfigOptions option, const std::string &name) {
    std::vector<FillingConfig> configList = iterateOverOption(desiredPattern, bestConfig, delta, steps, option);
    std::string initialValue = bestConfig.getConfigOption(option);

    bestConfig = findBestFillingForSeeds(desiredPattern, configList, minSeed, maxSeed, threads,
                                         name, option, initialValue);
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
    return bestConfig;
}


void FillingOptimization::optimizeSeeds(int multiplier) {
    std::cout << "Finding the best seed." << std::endl;
    std::vector<FillingConfig> configs = iterateOverSeeds(desiredPattern, bestConfig, minSeed, maxSeed * multiplier);
    bestConfig = selectBestConfigSingle(desiredPattern, configs, threads);
    std::cout << std::endl;
}


ConfigDisagreement configurationOptimizer(const DesiredPattern &desiredPattern, FillingConfig initialConfig,
                                          int minSeed, int maxSeed, int threads) {
    FillingOptimization optimization(desiredPattern, initialConfig, minSeed, maxSeed, threads);

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

    ConfigDisagreement bestFill(optimization.getBestConfig());
    bestFill.fillWithPatterns(desiredPattern);
    FillingConfig bestConfig = optimization.getBestConfig();
    bestConfig.printConfig();
    return bestFill;
}


ConfigDisagreement
seedOptimizer(const DesiredPattern &desiredPattern, FillingConfig initialConfig, int minSeed, int maxSeed,
              int threads) {
    FillingOptimization optimization(desiredPattern, initialConfig, minSeed, maxSeed, threads);

    optimization.optimizeSeeds(1);

    ConfigDisagreement bestFill(optimization.getBestConfig());
    bestFill.fillWithPatterns(desiredPattern);
    FillingConfig bestConfig = optimization.getBestConfig();
    bestConfig.printConfig();
    return bestFill;
}


void exportPatternToDirectory(FilledPattern pattern, const std::string &directorPath) {
    std::string resultsDirectory = directorPath + R"(\results\)";
    std::string patternDirectory = directorPath + R"(\results)";

    if (!fs::exists(resultsDirectory)) {
        fs::create_directory(resultsDirectory);
    }
    if (!fs::exists(patternDirectory)) {
        fs::create_directory(patternDirectory);
    }
//    CreateDirectory(resultsDirectory.c_str(), nullptr);
//    CreateDirectory(patternDirectory.c_str(), nullptr);
    pattern.exportToDirectory(patternDirectory);

    std::vector<std::vector<std::valarray<int>>> sortedPaths = getSortedPaths(pattern, startingPointNumber);
    export3DVectorToFile(sortedPaths, resultsDirectory, "best_paths");
}


void generalFinder(const std::string &directorPath, int minSeed, int maxSeed, int threads,
                   ConfigDisagreement (*optimizer)(const DesiredPattern &, FillingConfig, int, int, int)) {
    time_t startTime = clock();
    std::cout << "\n\nCurrent directory: " << directorPath << std::endl;
    DesiredPattern desiredPattern = openPatternFromDirectory(directorPath);
    std::string configPath = directorPath + "\\config.txt";
    FillingConfig initialConfig(configPath);

    ConfigDisagreement bestFilling = optimizer(desiredPattern, initialConfig, minSeed, maxSeed, threads);
    FillingConfig bestConfig = bestFilling.getConfig();
    bestConfig.exportConfig(directorPath);

    exportPatternToDirectory(bestFilling.getPattern(desiredPattern), directorPath);
    printf("Multi-thread execution time %.2f", (double) (clock() - startTime) / CLOCKS_PER_SEC);
}


void findBestConfig(const std::string &directorPath, int minSeed, int maxSeed, int threads) {
    generalFinder(directorPath, minSeed, maxSeed, threads, &configurationOptimizer);
}


void findBestSeed(const std::string &directorPath, int minSeed, int maxSeed, int threads) {
    generalFinder(directorPath, minSeed, maxSeed, threads, &seedOptimizer);
}


void recalculateBestConfig(const std::string &director_path) {
    time_t start_time = clock();
    std::cout << "\n\nCurrent directory: " << director_path << std::endl;
    DesiredPattern desired_pattern = openPatternFromDirectory(director_path);
    std::string config_path = director_path + R"(\results\best_config.txt)";
    FillingConfig config(config_path);

    ConfigDisagreement filling(config);
    filling.fillWithPatterns(desired_pattern);

    exportPatternToDirectory(filling.getPattern(desired_pattern), director_path);
    printf("Execution time %.2f", (double) (clock() - start_time) / CLOCKS_PER_SEC);
}