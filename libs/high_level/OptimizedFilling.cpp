//
// Created by zmmys on 05/11/2021.
//

#include "AutomaticPathGeneration.h"
#include "OpenFiles.h"
#include "OptimizedFilling.h"
#include "../pattern/FillingPatterns.h"
#include "../pattern/QuantifyPattern.h"

#include <iostream>
#include <omp.h>
#include <utility>
#include <vector>

//TODO make an iteration algorithm where we optimize for EACH filling method for a while, choose the best, and continue optimizing the best one

//const int initialIterationDepth = 2;
//const int fineIterationDepth = 4;


OptimizedFilling::OptimizedFilling(FillingConfig desiredConfig) :
        config(desiredConfig),
        disagreement(DBL_MAX) {}


void OptimizedFilling::fillWithPatterns(const DesiredPattern &desiredPattern) {
    FilledPattern pattern(desiredPattern, config);
    fillWithPaths(pattern);
    QuantifyPattern patternAgreement(pattern);
    disagreement = patternAgreement.calculateCorrectness(3, 1, 10, 1);
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
                                                double increment, int numberOfConfigs) {
    std::vector<FillingConfig> listOfConfigs;
    double initialRepulsion = initialConfig.getRepulsion();
    for (int i = 0; i <= numberOfConfigs; i++) {
        double newRepulsion = initialRepulsion + (i - (double) numberOfConfigs / 2) * increment;
        initialConfig.setConfigOption(Repulsion, std::to_string(newRepulsion));
        listOfConfigs.push_back(initialConfig);
    }
    return listOfConfigs;
}

std::vector<FillingConfig>
iterateOverStartingDistance(const DesiredPattern &desiredPattern, FillingConfig initialConfig,
                            double increment, int numberOfConfigs) {
    std::vector<FillingConfig> listOfConfigs;
    double initialStartingPointSeparation = initialConfig.getStartingPointSeparation();
    for (int i = 0; i <= numberOfConfigs; i++) {
        double newStartingPointSeparation =
                initialStartingPointSeparation + (i - (double) numberOfConfigs / 2) * increment;
        initialConfig.setConfigOption(StartingPointSeparation, std::to_string(newStartingPointSeparation));
        listOfConfigs.push_back(initialConfig);
    }
    return listOfConfigs;
}


std::vector<FillingConfig> iterateOverCollisionRadius(const DesiredPattern &desiredPattern, FillingConfig initialConfig,
                                                      int increment, int numberOfConfigs) {
    std::vector<FillingConfig> listOfConfigs;
    int initialCollisionRadius = initialConfig.getCollisionRadius();
    for (int i = 0; i <= numberOfConfigs; i++) {

        int newCollisionRadius = initialCollisionRadius + (int) (i - (double) numberOfConfigs / 2) * increment;
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
    }

    OptimizedFilling bestFill = selectBestFilling(filledPatterns);
    FillingConfig bestConfig = bestFill.getConfig();
    bestConfig.printConfig();
    printf("Lowest disagreement %.2f.\n", bestFill.getDisagreement());
    std::cout.flush();

    return bestFill;
}


std::vector<FillingConfig>
iterateOverSeeds(const DesiredPattern &desiredPattern, const std::vector<FillingConfig> &configList,
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



OptimizedFilling findBestFillingMethod(const DesiredPattern &desiredPattern, FillingConfig initialConfig,
                                       int minSeed, int maxSeed, int threads) {

//    std::vector<FillingConfig> configsWithDifferentMethods = iterateOverFillingMethod(desiredPattern, initialConfig);

    initialConfig.setConfigOption(Repulsion, "0.5");
    std::vector<FillingConfig> configsWithDifferentRepulsion = iterateOverRepulsion(desiredPattern, initialConfig, 0.1,
                                                                                    10);
    std::vector<FillingConfig> configsToTest = iterateOverSeeds(desiredPattern, configsWithDifferentRepulsion, minSeed,
                                                                maxSeed);

    OptimizedFilling bestFill = calculateFillsAndFindTheBestOne(desiredPattern, configsToTest, threads);
    FillingConfig bestRepulsionConfig = bestFill.getConfig();
    std::vector<FillingConfig> configsWithDifferentCollisionRadius = iterateOverCollisionRadius(desiredPattern,
                                                                                                bestRepulsionConfig, 1,
                                                                                                3);
    configsToTest = iterateOverSeeds(desiredPattern, configsWithDifferentCollisionRadius, minSeed, maxSeed);

    bestFill = calculateFillsAndFindTheBestOne(desiredPattern, configsToTest, threads);
    FillingConfig bestCollisionConfig = bestFill.getConfig();
    std::vector<FillingConfig> configsWithDifferentStartingDistance = iterateOverStartingDistance(desiredPattern,
                                                                                                  bestCollisionConfig,
                                                                                                  1, 5);
    configsToTest = iterateOverSeeds(desiredPattern, configsWithDifferentStartingDistance, minSeed, maxSeed);

    bestFill = calculateFillsAndFindTheBestOne(desiredPattern, configsToTest, threads);
    FillingConfig bestStartingConfig = bestFill.getConfig();

    configsWithDifferentRepulsion = iterateOverRepulsion(desiredPattern, bestStartingConfig, 0.02,
                                                                                    10);
    configsToTest = iterateOverSeeds(desiredPattern, configsWithDifferentRepulsion, minSeed,
                                                                maxSeed);

    bestFill = calculateFillsAndFindTheBestOne(desiredPattern, configsToTest, threads);
    bestRepulsionConfig = bestFill.getConfig();
    configsWithDifferentCollisionRadius = iterateOverCollisionRadius(desiredPattern,
                                                                                                bestRepulsionConfig, 1,
                                                                                                3);
    configsToTest = iterateOverSeeds(desiredPattern, configsWithDifferentCollisionRadius, minSeed, maxSeed);

    bestFill = calculateFillsAndFindTheBestOne(desiredPattern, configsToTest, threads);
    bestCollisionConfig = bestFill.getConfig();
    configsWithDifferentStartingDistance = iterateOverStartingDistance(desiredPattern,
                                                                                                  bestCollisionConfig,
                                                                                                  1, 5);
    configsToTest = iterateOverSeeds(desiredPattern, configsWithDifferentStartingDistance, minSeed, maxSeed);

    bestFill = calculateFillsAndFindTheBestOne(desiredPattern, configsToTest, threads);

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
