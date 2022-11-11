//
// Created by Michał Zmyślony on 14/01/2022.
//

#include "ReadingFromOutside.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include "OpenFiles.h"


FillingOptimization
configurationOptimizerFromString(FillingOptimization &optimization, const std::string &configuration) {
    std::string line;
    std::stringstream command(configuration);
    std::string element;
    std::vector<std::string> row;

    while (std::getline(command, element, ',')) {
        row.push_back(element);
    }

    if (row.size() == 3) {
        ConfigOptions option = stringToConfig(row[0]);
        int steps = stoi(row[1]);
        double delta = stod(row[2]);

        switch (option) {
            case Repulsion:
                optimization.optimizeRepulsion(delta, steps);
                return optimization;
            case StartingPointSeparation:
                optimization.optimizeStartingDistance(delta, steps);
                return optimization;
            case CollisionRadius:
                optimization.optimizeCollisionRadius(delta, steps);
                return optimization;
            default:
                std::cout << "Unrecognised optimization command: " << row[0] << std::endl;
                return optimization;
        }
    } else if (row.size() == 2) {
        ConfigOptions option = stringToConfig(row[0]);
        int multiplier = stoi(row[1]);

        switch (option) {
            case Seed:
                optimization.optimizeSeeds(multiplier);
                return optimization;
            default:
                std::cout << "Unrecognised optimization command: " << row[0] << std::endl;
                return optimization;
        }
    }
    std::cout << "Unrecognised optimization command length:" << row.size() << std::endl;
    return optimization;
}


FillingOptimization optimizeFromFile(FillingOptimization &optimization, const std::string &filepath) {
    std::string command;
    std::fstream file(filepath);

    while (std::getline(file, command)) {
        configurationOptimizerFromString(optimization, command);
    }
    return optimization;
}


ConfigDisagreement configurationOptimizer(const DesiredPattern &desiredPattern, FillingConfig initialConfig,
                                          int minSeed, int maxSeed, int threads,
                                          const std::string &optimizerConfigPath) {
    FillingOptimization optimization(desiredPattern, initialConfig, minSeed, maxSeed, threads);

    optimizeFromFile(optimization, optimizerConfigPath);

    ConfigDisagreement bestFill(optimization.getBestConfig());
    bestFill.fillWithPatterns(desiredPattern);
    FillingConfig bestConfig = optimization.getBestConfig();
    bestConfig.printConfig();
    return bestFill;
}


void generalFinderString(const std::string &directorPath, int minSeed, int maxSeed, int threads,
                         const std::string &optimizerPath) {
    time_t startTime = clock();
    std::cout << "\n\nCurrent directory: " << directorPath << std::endl;
    DesiredPattern desiredPattern = openPatternFromDirectory(directorPath);
    std::string configPath = directorPath + "\\config.txt";
    FillingConfig initialConfig(configPath);

    ConfigDisagreement bestFilling = configurationOptimizer(desiredPattern, initialConfig, minSeed, maxSeed,
                                                            threads,
                                                            optimizerPath);
    FillingConfig bestConfig = bestFilling.getConfig();
    bestConfig.exportConfig(directorPath);

    exportPatternToDirectory(bestFilling.getPattern(desiredPattern), directorPath);
    printf("Multi-thread execution time %.2f", (double) (clock() - startTime) / CLOCKS_PER_SEC);
}