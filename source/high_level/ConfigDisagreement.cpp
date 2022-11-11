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