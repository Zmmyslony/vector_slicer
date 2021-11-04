//
// Created by Michał Zmyślony on 21/10/2021.
//

#include "AutomaticPathGeneration.h"
#include "../pattern/FillingPatterns.h"
#include "../gui/OpenFiles.h"
#include "../pattern/QuantifyPattern.h"
#include <ctime>
#include <windows.h>
#include "../pattern/GcodeGenerator.h"
#include <iostream>
#include "ProgressBar.h"


FilledPattern generateAPrintPattern(std::string directorPath, DesiredPattern desiredPattern, int seed) {
    FilledPattern pattern = openFilledPatternFromDirectoryAndPattern(directorPath, desiredPattern, seed);
    fillWithPaths(pattern);

    return pattern;
}


void exportPatternToDirectory(const FilledPattern &pattern, const std::string &directorPath, const int &seed) {
    std::string resultsDirectory = directorPath + R"(\results\)";
    std::string patternDirectory = directorPath + R"(\results\seed_)" + std::to_string(seed);

    CreateDirectory(resultsDirectory.c_str(), nullptr);
    CreateDirectory(patternDirectory.c_str(), nullptr);
    pattern.exportToDirectory(patternDirectory);
}


void findBestPath(FilledPattern &pattern) {
    GcodeGenerator gcodeBase(pattern);
//    printf("Created base.\n");
    gcodeBase.findBestStartingPoints();
}


void generateAndExportPrintPattern(const std::string &directorPath, const DesiredPattern &desiredPattern, int seed) {
    FilledPattern pattern = generateAPrintPattern(directorPath, desiredPattern, seed);
//    printf("Best pattern generated.\n");
//    findBestPath(pattern);
//    printf("Best path found.\n");
    exportPatternToDirectory(pattern, directorPath, seed);
}


void generatePrintPattern(std::string &directorPath, int minSeed, int maxSeed) {
    std::cout << "\nCurrent directory: " << directorPath << std::endl;
    DesiredPattern desiredPattern = openPatternFromDirectory(directorPath);

    int bestSeed = 0;
    double bestDisagreement = 1000;
    for (int currentSeed = minSeed; currentSeed <= maxSeed; currentSeed++) {
        showProgress(currentSeed - minSeed, maxSeed - minSeed);
        FilledPattern testPattern = generateAPrintPattern(directorPath, desiredPattern, currentSeed);
        QuantifyPattern patternAgreement(testPattern);

        double currentDisagreement = patternAgreement.calculateCorrectness(5, 0.2, 1, 1);
        if (currentDisagreement < bestDisagreement) {
            bestSeed = currentSeed;
            bestDisagreement = currentDisagreement;
        }
    }
    std::cout << std::endl;
    printf("%i seeds from %i to %i were tested. Seed %i had the lowest disagreement of %.2f\n",
           maxSeed - minSeed + 1, minSeed, maxSeed, bestSeed, bestDisagreement);
    generateAndExportPrintPattern(directorPath, desiredPattern, bestSeed);
}

