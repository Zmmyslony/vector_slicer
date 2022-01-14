//
// Created by Michał Zmyślony on 14/01/2022.
//

#include "ReadingFromOutside.h"
#include <sstream>
#include <fstream>
#include <iostream>

FillingOptimization configurationOptimizerFromString(FillingOptimization &optimization, const std::string &configuration) {
    std::vector<std::vector<int>> table;
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
            case Seed:
                optimization.optimizeSeeds(steps);
                return optimization;
            default:
                return optimization;
        }
    }
    std::cout << "Unrecognised optimization command." << std::endl;
    return optimization;
}


FillingOptimization optimizeFromFile(FillingOptimization &optimization, const std::string &filepath) {
    std::vector<std::vector<int>> table;
    std::string command;
    std::fstream file(filepath);

    while (std::getline(file, command)) {
        configurationOptimizerFromString(optimization, command);
    }
    return optimization;
}


ConfigDisagreement configurationOptimizer(const DesiredPattern &desiredPattern, FillingConfig initialConfig,
                                          int minSeed, int maxSeed, int threads, const std::string &optimizerConfigPath) {
    FillingOptimization optimization(desiredPattern, initialConfig, minSeed, maxSeed, threads);

    optimizeFromFile(optimization, optimizerConfigPath);

    ConfigDisagreement bestFill(optimization.getBestConfig());
    bestFill.fillWithPatterns(desiredPattern);
    FillingConfig bestConfig = optimization.getBestConfig();
    bestConfig.printConfig();
    return bestFill;
}