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
        configOptions option = stringToConfig(row[0]);
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
        configOptions option = stringToConfig(row[0]);
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


FillingOptimization optimizeFromFile(FillingOptimization &optimization, const fs::path &filepath) {
    std::string command;
    std::fstream file(filepath.string());

    while (std::getline(file, command)) {
        configurationOptimizerFromString(optimization, command);
    }
    return optimization;
}


ConfigDisagreement configurationOptimizer(const DesiredPattern &desired_pattern, FillingConfig initial_config,
                                          int min_seed, int max_seed, int threads,
                                          const std::string &optimizer_config_path) {
    FillingOptimization optimization(desired_pattern, initial_config, min_seed, max_seed, threads);

    optimizeFromFile(optimization, optimizer_config_path);

    ConfigDisagreement best_fill(optimization.getBestConfig());
    best_fill.fillWithPatterns(desired_pattern);
    FillingConfig best_config = optimization.getBestConfig();
    best_config.printConfig();
    return best_fill;
}


void generalFinderString(const fs::path &pattern_directory, int min_seed, int max_seed, int threads,
                         const std::string &optimizer_path) {
    time_t start_time = clock();
    std::cout << "\n\nCurrent directory: " << pattern_directory << std::endl;
    DesiredPattern desired_pattern = openPatternFromDirectory(pattern_directory);
    fs::path config_path = pattern_directory / "config.txt";
    FillingConfig initial_config(config_path);

    ConfigDisagreement best_filling = configurationOptimizer(desired_pattern, initial_config, min_seed, max_seed,
                                                             threads,
                                                             optimizer_path);
    FillingConfig best_config = best_filling.getConfig();
    best_config.exportConfig(pattern_directory);

    exportPatternToDirectory(best_filling.getPattern(desired_pattern), pattern_directory);
    printf("Multi-thread execution time %.2f", (double) (clock() - start_time) / CLOCKS_PER_SEC);
}