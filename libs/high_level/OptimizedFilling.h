//
// Created by Michał Zmyślony on 05/11/2021.
//

#ifndef VECTOR_SLICER_OPTIMIZEDFILLING_H
#define VECTOR_SLICER_OPTIMIZEDFILLING_H

#include "../auxiliary/FillingConfig.h"
#include "../pattern/FilledPattern.h"
#include "../pattern/DesiredPattern.h"
#include <string>

void findBestConfig(const std::string &directorPath, int minSeed, int maxSeed, int threads);

class OptimizedFilling {
    FillingConfig config;
    double disagreement;

public:
    const FillingConfig &getConfig() const;

    FilledPattern getPattern(const DesiredPattern &desiredPattern) const;

    void fillWithPatterns(const DesiredPattern &desiredPattern);

    double getDisagreement() const;

    explicit OptimizedFilling(FillingConfig config);
};

#endif //VECTOR_SLICER_OPTIMIZEDFILLING_H
