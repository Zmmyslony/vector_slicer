//
// Created by Michał Zmyślony on 05/11/2021.
//


#include "ConfigDisagreement.h"
#include "../pattern/FillingPatterns.h"
#include "../pattern/QuantifyPattern.h"

//#include <iostream>
#include <vector>
#include <cfloat>

ConfigDisagreement::ConfigDisagreement(FillingConfig desiredConfig) :
        config(desiredConfig),
        disagreement(DBL_MAX) {}


void ConfigDisagreement::fillWithPatterns(const DesiredPattern &desiredPattern) {
    FilledPattern pattern(desiredPattern, config);
    fillWithPaths(pattern);
    QuantifyPattern patternAgreement(pattern);
//    disagreement = patternAgreement.calculateCorrectness(10, 2, 1000, 10,
//                                                         1, 1, 2, 2);
//    disagreement = patternAgreement.calculateCorrectness(10, 2, 10000, 10,
//                                                         1, 1, 2, 2);
//    disagreement = patternAgreement.calculateCorrectness(10, 0.2, 10000, 10,
//                                                         1, 1, 2, 2);
    disagreement = patternAgreement.calculateCorrectness(10, 8, 100, 10,
                                                         1, 1, 2,2);

}


const FillingConfig &ConfigDisagreement::getConfig() const {
    return config;
}

FilledPattern ConfigDisagreement::getPattern(const DesiredPattern &desiredPattern) const {
    FilledPattern pattern(desiredPattern, config);
    fillWithPaths(pattern);
    return pattern;
}

double ConfigDisagreement::getDisagreement() const {
    return disagreement;
}