//
// Created by zmmys on 05/11/2021.
//

#include "OpenFiles.h"
#include "OptimizePathGeneration.h"
#include "../pattern/FillingPatterns.h"
#include "../pattern/QuantifyPattern.h"

FillingConfig findBestConfig(const std::string &directorPath, int threads, int iterationDepth, int seed) {
    DesiredPattern desiredPattern = openPatternFromDirectory(directorPath);
    std::string configPath = directorPath + "\\config.txt";
    FillingConfig initialConfig(configPath);

    FilledPattern pattern(desiredPattern, initialConfig, seed);
    fillWithPaths(pattern);
    QuantifyPattern patternAgreement(pattern);
    return initialConfig;
}

