// Copyright (c) 2021-2023, Michał Zmyślony, mlz22@cam.ac.uk.
//
// Please cite Michał Zmyślony and Dr John Biggins if you use any part of this code in work you publish or distribute.
//
// This file is part of Vector Slicer.
//
// Vector Slicer is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
// later version.
//
// Vector Slicer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License along with Vector Slicer.
// If not, see <https://www.gnu.org/licenses/>.

//
// Created by Michał Zmyślony on 05/11/2021.
//

#define _USE_MATH_DEFINES
#ifndef VECTOR_SLICER_FILLING_CONFIG_H
#define VECTOR_SLICER_FILLING_CONFIG_H

#include <cmath>
#include <math.h>
#include <string>
#include <vector>
#include <boost/filesystem/path.hpp>

namespace fs = boost::filesystem;


enum fillingMethod {
    Splay, Perimeter, Dual
};
enum configOptions {
    InitialFillingMethod,
    CollisionRadius,
    StepLength,
    PrintRadius,
    Repulsion,
    StartingPointSeparation,
    Seed,
    RepulsionRadius,
    RepulsionAngle
};

/// The generating parameters that decide how the pattern will be filled.
class FillingConfig {
    /// Filling method can be either Perimeter of Dual. Perimeter extracts the boundary and selects points out of it
    /// that are distanced starting_point_separation away from each other. The Dual method selects points and creates
    /// a Dual line to it (perpendicular to the vector field) that is also separated into the list of equidistant points.
    fillingMethod filling_method;
    /// Distance between the new point and the nearest filled point deciding when the line will terminate.
    double collision_radius;
    /// Multiplier deciding how much the lines will try to avoid one another.
    double repulsion;
    /// Length of each step in pixels.
    int step_length;
    /// See FillingMethod
    double starting_point_separation;
    /// Radius away from the generated line where the pixels of the filled_matrix will be filled.
    double print_radius;
    /// Numerical seed deciding the randomness of the whole algorithm.
    unsigned int seed;
    /// Legacy. Additional radius away from the print_radius that ought to be checked for other filled points.
    double repulsion_radius;

    double repulsion_angle = M_PI;

    void readLineOfConfig(std::vector<std::string> line);

public:
    void printConfig();

    void setConfigOption(const configOptions &option, const std::string &value);

    [[nodiscard]] fillingMethod getInitialFillingMethod() const;

    [[nodiscard]] double getCollisionRadius() const;

    [[nodiscard]] double getRepulsion() const;

    [[nodiscard]] double getStartingPointSeparation() const;

    [[nodiscard]] double getPrintRadius() const;

    [[nodiscard]] unsigned int getSeed() const;

    [[nodiscard]] double getRepulsionRadius() const;

    [[nodiscard]] double getRepulsionAngle() const;

    explicit FillingConfig();

    explicit FillingConfig(const fs::path &config_path);

    FillingConfig(const FillingConfig &source_config, int source_seed);

    FillingConfig(fillingMethod new_perimeter_filling_method, int new_collision_radius,
                  int new_starting_point_separation, double new_repulsion, int new_step_length,
                  int new_print_radius, double new_repulsion_radius, unsigned int new_seed);


    void exportConfig(const fs::path &directory, const std::string &suffix);

    [[nodiscard]] int getStepLength() const;

    std::string getConfigOption(configOptions option);
};

configOptions stringToConfig(const std::string &string_option);

void exportConfigList(const std::vector<FillingConfig> &configs, fs::path path);

std::vector<FillingConfig> readMultiSeedConfig(const fs::path &config_path);

#endif //VECTOR_SLICER_FILLING_CONFIG_H
