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
// Created by Michał Zmyślony on 10/01/2022.
//

#ifndef VECTOR_SLICER_FILLINGOPTIMIZATION_H
#define VECTOR_SLICER_FILLINGOPTIMIZATION_H

#include "ConfigDisagreement.h"

class FillingOptimization {
    int threads;
    int min_seed;
    int max_seed;

    DesiredPattern desired_pattern;
    FillingConfig best_config;
public:
    const FillingConfig &getBestConfig() const;

private:

    void optimizeOption(double delta, int steps, configOptions option, const std::string &name);

public:

    FillingOptimization(DesiredPattern desired_pattern, const FillingConfig &initial_config, int min_seed, int max_seed,
                        int threads);

    explicit FillingOptimization(DesiredPattern desired_pattern, const FillingConfig &initial_config);

    void optimizeRepulsion(double delta, int steps);

    void optimizeStartingDistance(double delta, int steps);

    void optimizeCollisionRadius(double delta, int steps);

    void optimizeSeeds(int multiplier);
};

void findBestSeed(const fs::path &pattern_path, int min_seed, int max_seed, int threads);

void exportPatternToDirectory(FilledPattern pattern, const boost::filesystem::path &pattern_path);

#endif //VECTOR_SLICER_FILLINGOPTIMIZATION_H
