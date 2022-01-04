//
// Created by Michał Zmyślony on 05/11/2021.
//

//#include "AutomaticPathGeneration.h"

#include "OpenFiles.h"
#include "OptimizedFilling.h"
#include "../pattern/FillingPatterns.h"
#include "../pattern/QuantifyPattern.h"


#include <iostream>
#include <omp.h>
#include <utility>
#include <vector>
#include <windows.h>


OptimizedFilling::OptimizedFilling(FillingConfig desiredConfig) :
        config(desiredConfig),
        disagreement(DBL_MAX) {}


void OptimizedFilling::fillWithPatterns(const DesiredPattern &desiredPattern) {
    FilledPattern pattern(desiredPattern, config);
    fillWithPaths(pattern);
    QuantifyPattern patternAgreement(pattern);
    disagreement = patternAgreement.calculateCorrectness(5, 4, 1000, 1, 1, 2, 2, 2);
}


void exportPatternToDirectory(const FilledPattern &pattern, const std::string &directorPath, const int &seed) {
    std::string resultsDirectory = directorPath + R"(\results\)";
    std::string patternDirectory = directorPath + R"(\results\seed_best)";
    CreateDirectory(resultsDirectory.c_str(), nullptr);
    CreateDirectory(patternDirectory.c_str(), nullptr);
    pattern.exportToDirectory(patternDirectory);
}


const FillingConfig &OptimizedFilling::getConfig() const {
    return config;
}

FilledPattern OptimizedFilling::getPattern(const DesiredPattern &desiredPattern) const {
    FilledPattern pattern(desiredPattern, config);
    fillWithPaths(pattern);
    return pattern;
}

double OptimizedFilling::getDisagreement() const {
    return disagreement;
}


std::vector<FillingConfig> iterateOverFillingMethod(const DesiredPattern &desiredPattern, FillingConfig initialConfig) {
    std::vector<FillingConfig> listOfConfigs;
    std::vector<std::string> allFillingMethods = {"RandomRadial", "ConsecutiveRadial", "RandomPerimeter",
                                                  "ConsecutivePerimeter"};
    initialConfig.setConfigOption(Repulsion, std::to_string(0));
    for (auto &fillingMethod: allFillingMethods) {
        initialConfig.setConfigOption(InitialFillingMethod, fillingMethod);
        listOfConfigs.push_back(initialConfig);
    }
    return listOfConfigs;
}


std::vector<FillingConfig> iterateOverRepulsion(const DesiredPattern &desiredPattern, FillingConfig initialConfig,
                                                double delta, int numberOfConfigs) {
    std::vector<FillingConfig> listOfConfigs;
    double increment = delta / numberOfConfigs;
    double initialRepulsion = initialConfig.getRepulsion();
    for (int i = -numberOfConfigs; i <= numberOfConfigs; i++) {
        double newRepulsion = initialRepulsion + i * increment;
        initialConfig.setConfigOption(Repulsion, std::to_string(newRepulsion));
        listOfConfigs.push_back(initialConfig);
    }
    return listOfConfigs;
}

std::vector<FillingConfig>
iterateOverStartingDistance(const DesiredPattern &desiredPattern, FillingConfig initialConfig,
                            double delta, int numberOfConfigs) {
    std::vector<FillingConfig> listOfConfigs;
    double increment = delta / numberOfConfigs;
    double initialStartingPointSeparation = initialConfig.getStartingPointSeparation();
    for (int i = -numberOfConfigs; i <= numberOfConfigs; i++) {
        double newStartingPointSeparation = initialStartingPointSeparation + i * increment;
        initialConfig.setConfigOption(StartingPointSeparation, std::to_string(newStartingPointSeparation));
        listOfConfigs.push_back(initialConfig);
    }
    return listOfConfigs;
}


std::vector<FillingConfig> iterateOverCollisionRadius(const DesiredPattern &desiredPattern, FillingConfig initialConfig,
                                                      double delta, int numberOfConfigs) {
    std::vector<FillingConfig> listOfConfigs;
    double increment = delta / numberOfConfigs;
    double initialCollisionRadius = initialConfig.getCollisionRadius();
    for (int i = -numberOfConfigs; i <= numberOfConfigs; i++) {
        double newCollisionRadius = initialCollisionRadius + i * increment;
        if (newCollisionRadius > 0) {
            initialConfig.setConfigOption(CollisionRadius, std::to_string(newCollisionRadius));
            listOfConfigs.push_back(initialConfig);
        }
    }
    return listOfConfigs;
}


std::vector<FillingConfig> iterateOverSeeds(const DesiredPattern &desiredPattern, FillingConfig initialConfig,
                                            int minSeed, int maxSeed) {
    std::vector<FillingConfig> listOfConfigs;
    for (int i = minSeed; i <= maxSeed; i++) {
        initialConfig.setConfigOption(Seed, std::to_string(i));
        listOfConfigs.push_back(initialConfig);
    }
    return listOfConfigs;
}


std::vector<FillingConfig>
generateConfigWithSeeds(const DesiredPattern &desiredPattern, const std::vector<FillingConfig> &configList,
                        int minSeed, int maxSeed) {
    std::vector<FillingConfig> configsToTest;
    for (auto &configMethod: configList) {
        std::vector<FillingConfig> configsWithDifferentSeeds = iterateOverSeeds(desiredPattern, configMethod, minSeed,
                                                                                maxSeed);
        for (auto &configSeed: configsWithDifferentSeeds) {
            configsToTest.push_back(configSeed);

        }
    }
    return configsToTest;
}


OptimizedFilling selectBestFilling(const std::vector<OptimizedFilling> &filledPatterns) {
    double lowestDisagreement = DBL_MAX;
    OptimizedFilling bestFill = filledPatterns[0];

    for (auto &optimizedConfig: filledPatterns) {
        double currentDisagreement = optimizedConfig.getDisagreement();
        if (currentDisagreement < lowestDisagreement) {
            lowestDisagreement = currentDisagreement;
            bestFill = optimizedConfig;
        }
    }
    return bestFill;
}


OptimizedFilling
calculateFillsAndFindTheBestOne(const DesiredPattern &desiredPattern, const std::vector<FillingConfig> &listOfConfigs,
                                int threads) {
    std::vector<OptimizedFilling> filledPatterns;

    for (auto &config: listOfConfigs) {
        filledPatterns.emplace_back(config);
    }

    omp_set_num_threads(threads);
#pragma omp parallel for
    for (int i = 0; i < listOfConfigs.size(); i++) {
        filledPatterns[i].fillWithPatterns(desiredPattern);
        std::cout.flush();
    }

    OptimizedFilling bestFill = selectBestFilling(filledPatterns);
    return bestFill;
}


FillingConfig findBestConfigForSeeds(const DesiredPattern &desiredPattern, const std::vector<FillingConfig> &configList,
                                     int minSeed, int maxSeed, int threads) {
    std::vector<FillingConfig> configsToTest = generateConfigWithSeeds(desiredPattern, configList, minSeed, maxSeed);

    OptimizedFilling bestFill = calculateFillsAndFindTheBestOne(desiredPattern, configsToTest, threads);
    FillingConfig bestConfig = bestFill.getConfig();

    return bestConfig;
}


FillingConfig
findBestFillingForSeeds(const DesiredPattern &desiredPattern, const std::vector<FillingConfig> &configList,
                        int minSeed, int maxSeed, int threads, const std::string &type, ConfigOptions optimizedOption,
                        const std::string &initialValue) {
    std::cout << "Optimizing over " << type << ". " << std::endl;

    std::vector<FillingConfig> configsToTest = generateConfigWithSeeds(desiredPattern, configList, minSeed, maxSeed);
    OptimizedFilling bestFill = calculateFillsAndFindTheBestOne(desiredPattern, configsToTest, threads);
    FillingConfig config = bestFill.getConfig();
    std::cout << "\tInitial value: " << stod(initialValue) << ". Optimized value: "
              << stod(config.getConfigOption(optimizedOption)) << std::endl;
    printf("\tDisagreement %.3f.", bestFill.getDisagreement());
    std::cout << std::endl << std::endl;
    return config;
}


FillingConfig findBestRepulsion(const DesiredPattern &desiredPattern, FillingConfig initialConfig,
                                int minSeed, int maxSeed, int threads, double delta, int numberOfConfigs) {
    std::vector<FillingConfig> configList = iterateOverRepulsion(desiredPattern, initialConfig, delta,
                                                                 numberOfConfigs);
    std::string initialValue = initialConfig.getConfigOption(Repulsion);

    FillingConfig bestConfig = findBestFillingForSeeds(desiredPattern, configList, minSeed, maxSeed, threads,
                                                       "repulsion", Repulsion, initialValue);
    return bestConfig;
}


FillingConfig findBestCollisionRadius(const DesiredPattern &desiredPattern, FillingConfig initialConfig,
                                      int minSeed, int maxSeed, int threads, double delta, int numberOfConfigs) {
    std::vector<FillingConfig> configList = iterateOverCollisionRadius(desiredPattern, initialConfig, delta,
                                                                       numberOfConfigs);
    std::string initialValue = initialConfig.getConfigOption(CollisionRadius);

    FillingConfig bestConfig = findBestFillingForSeeds(desiredPattern, configList, minSeed, maxSeed, threads,
                                                       "collision radius", CollisionRadius, initialValue);
    return bestConfig;
}


FillingConfig findBestStartingDistance(const DesiredPattern &desiredPattern, FillingConfig initialConfig,
                                       int minSeed, int maxSeed, int threads, double delta, int numberOfConfigs) {
    std::vector<FillingConfig> configList = iterateOverStartingDistance(desiredPattern, initialConfig, delta,
                                                                        numberOfConfigs);

    std::string initialValue = initialConfig.getConfigOption(StartingPointSeparation);

    FillingConfig bestConfig = findBestFillingForSeeds(desiredPattern, configList, minSeed, maxSeed, threads,
                                                       "starting point separation", StartingPointSeparation,
                                                       initialValue);
    return bestConfig;
}


OptimizedFilling findBestFillingMethod(const DesiredPattern &desiredPattern, FillingConfig initialConfig,
                                       int minSeed, int maxSeed, int threads) {

//    initialConfig.setConfigOption(Repulsion, "0.5");
    FillingConfig bestConfig = initialConfig;
//    bestConfig.setConfigOption(InitialFillingMethod, "RandomRadial");

//    bestConfig.setConfigOption(StartingPointSeparation, "18");
//    bestConfig.setConfigOption(Repulsion, "1.25");
//    bestConfig.setConfigOption(CollisionRadius, "3");
//    bestConfig = findBestRepulsion(desiredPattern, bestConfig, 4, maxSeed, threads, 0.00, 1);

    bestConfig = findBestStartingDistance(desiredPattern, bestConfig, minSeed, maxSeed, threads, 8, 6);
//    bestConfig = findBestRepulsion(desiredPattern, bestConfig, minSeed, maxSeed, threads, 0.5, 4);
    bestConfig = findBestCollisionRadius(desiredPattern, bestConfig, minSeed, maxSeed, threads, 4, 4);

    bestConfig = findBestStartingDistance(desiredPattern, bestConfig, minSeed, maxSeed, threads, 4, 6);
//    bestConfig = findBestRepulsion(desiredPattern, bestConfig, minSeed, maxSeed, threads, 0.25, 4);
    bestConfig = findBestCollisionRadius(desiredPattern, bestConfig, minSeed, maxSeed, threads, 2, 4);

    bestConfig = findBestStartingDistance(desiredPattern, bestConfig, minSeed, maxSeed, threads, 2, 6);
//    bestConfig = findBestRepulsion(desiredPattern, bestConfig, minSeed, maxSeed, threads, 0.125, 4);
    bestConfig = findBestCollisionRadius(desiredPattern, bestConfig, minSeed, maxSeed, threads, 1, 4);

    std::cout << "Finding the best seed." << std::endl;
    bestConfig = findBestConfigForSeeds(desiredPattern, {bestConfig}, minSeed, 10 * maxSeed, threads);

    OptimizedFilling bestFill(bestConfig);
    bestFill.fillWithPatterns(desiredPattern);
    bestConfig.printConfig();
    return bestFill;
}


void findBestConfig(const std::string &directorPath, int minSeed, int maxSeed, int threads) {
    time_t startTime = clock();
    std::cout << "\n\nCurrent directory: " << directorPath << std::endl;
    DesiredPattern desiredPattern = openPatternFromDirectory(directorPath);
    std::string configPath = directorPath + "\\config.txt";
    FillingConfig initialConfig(configPath);

    OptimizedFilling bestFilling = findBestFillingMethod(desiredPattern, initialConfig, minSeed, maxSeed, threads);

    exportPatternToDirectory(bestFilling.getPattern(desiredPattern), directorPath, 0);
    printf("Multi-thread execution time %.2f", (double) (clock() - startTime) / CLOCKS_PER_SEC);
}
