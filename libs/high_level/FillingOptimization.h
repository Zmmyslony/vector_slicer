//
// Created by Michał Zmyślony on 10/01/2022.
//

#ifndef VECTOR_SLICER_FILLINGOPTIMIZATION_H
#define VECTOR_SLICER_FILLINGOPTIMIZATION_H

#include "ConfigDisagreement.h"

class FillingOptimization {
    int threads;
    int minSeed;
    int maxSeed;

    DesiredPattern desiredPattern;
    FillingConfig bestConfig;
public:
    const FillingConfig &getBestConfig() const;

private:

    void optimizeOption(double delta, int steps, ConfigOptions option, const std::string &name);

public:

    FillingOptimization(DesiredPattern desiredPattern, const FillingConfig &initialConfig, int minSeed, int maxSeed,
                        int threads);

    explicit FillingOptimization(DesiredPattern desiredPattern, const FillingConfig &initialConfig);

    void optimizeRepulsion(double delta, int steps);

    void optimizeStartingDistance(double delta, int steps);

    void optimizeCollisionRadius(double delta, int steps);

    void optimizeSeeds(int multiplier);
};

void findBestSeed(const std::string &directorPath, int minSeed, int maxSeed, int threads);

#endif //VECTOR_SLICER_FILLINGOPTIMIZATION_H
