//
// Created by Michał Zmyślony on 10/01/2022.
//

#include "FillingOptimization.h"
#include "ConfigGeneration.h"
#include <omp.h>
#include <iostream>
#include <utility>


#include "OpenFiles.h"
#include "../pattern/FillingPatterns.h"
#include "../pattern/IndexedPath.h"
#include "../auxiliary/Exporting.h"

#include <vector>
#include <windows.h>

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


ConfigDisagreement
calculateFillsAndFindTheBestOne(const DesiredPattern &desiredPattern, const std::vector<FillingConfig> &listOfConfigs,
                                int threads) {
    std::vector<ConfigDisagreement> filledPatterns;

    for (auto &config: listOfConfigs) {
        filledPatterns.emplace_back(config);
    }

    omp_set_num_threads(threads);
#pragma omp parallel for
    for (int i = 0; i < listOfConfigs.size(); i++) {
        filledPatterns[i].fillWithPatterns(desiredPattern);
        std::cout.flush();
    }

    ConfigDisagreement bestFill = selectBestFilling(filledPatterns);
    return bestFill;
}


FillingConfig
findBestFillingForSeeds(const DesiredPattern &desiredPattern, const std::vector<FillingConfig> &configList,
                        int minSeed, int maxSeed, int threads, const std::string &type, ConfigOptions optimizedOption,
                        const std::string &initialValue) {
    std::cout << "Optimizing over " << type << ". " << std::endl;

    std::vector<FillingConfig> configsToTest = iterateOverSeeds(desiredPattern, configList, minSeed, maxSeed);
    ConfigDisagreement bestFill = calculateFillsAndFindTheBestOne(desiredPattern, configsToTest, threads);
    FillingConfig config = bestFill.getConfig();
    std::cout << "\tInitial value: " << stod(initialValue) << ". Optimized value: "
              << stod(config.getConfigOption(optimizedOption)) << std::endl;
    printf("\tDisagreement %.3f.", bestFill.getDisagreement());
    std::cout << std::endl << std::endl;
    return config;
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
    optimizeOption(delta, steps, StartingPointSeparation, "starting point separation");
}

const FillingConfig &FillingOptimization::getBestConfig() const {
    return bestConfig;
}

void FillingOptimization::optimizeSeeds(int multiplier) {
    std::cout << "Finding the best seed." << std::endl;
    std::vector<FillingConfig> configs = iterateOverSeeds(desiredPattern, bestConfig, minSeed, maxSeed * multiplier);
    ConfigDisagreement bestFill = calculateFillsAndFindTheBestOne(desiredPattern, configs, threads);
    bestConfig = bestFill.getConfig();
    printf("\tDisagreement %.3f.", bestFill.getDisagreement());
    std::cout << std::endl << std::endl;
}

ConfigDisagreement findBestFillingMethod(const DesiredPattern &desiredPattern, FillingConfig initialConfig,
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


void exportPatternToDirectory(FilledPattern pattern, const std::string &directorPath) {
    std::string resultsDirectory = directorPath + R"(\results\)";
    std::string patternDirectory = directorPath + R"(\results)";
    CreateDirectory(resultsDirectory.c_str(), nullptr);
    CreateDirectory(patternDirectory.c_str(), nullptr);
    pattern.exportToDirectory(patternDirectory);

    std::vector<std::vector<std::valarray<int>>> sortedPaths = getSortedPaths(pattern, 5);
    export3DVectorToFile(sortedPaths, resultsDirectory, "best_paths");
}


void findBestConfig(const std::string &directorPath, int minSeed, int maxSeed, int threads) {
    time_t startTime = clock();
    std::cout << "\n\nCurrent directory: " << directorPath << std::endl;
    DesiredPattern desiredPattern = openPatternFromDirectory(directorPath);
    std::string configPath = directorPath + "\\config.txt";
    FillingConfig initialConfig(configPath);

    ConfigDisagreement bestFilling = findBestFillingMethod(desiredPattern, initialConfig, minSeed, maxSeed, threads);
    FillingConfig bestConfig = bestFilling.getConfig();
    bestConfig.exportConfig(directorPath);

    exportPatternToDirectory(bestFilling.getPattern(desiredPattern), directorPath);
    printf("Multi-thread execution time %.2f", (double) (clock() - startTime) / CLOCKS_PER_SEC);
}


void recalculateBestConfig(const std::string &directorPath) {
    time_t startTime = clock();
    std::cout << "\n\nCurrent directory: " << directorPath << std::endl;
    DesiredPattern desiredPattern = openPatternFromDirectory(directorPath);
    std::string configPath = directorPath + R"(\results\best_config.txt)";
    FillingConfig config(configPath);

    ConfigDisagreement filling(config);
    filling.fillWithPatterns(desiredPattern);

    exportPatternToDirectory(filling.getPattern(desiredPattern), directorPath);
    printf("Execution time %.2f", (double) (clock() - startTime) / CLOCKS_PER_SEC);
}