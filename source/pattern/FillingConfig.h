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

#ifndef VECTOR_SLICER_FILLINGCONFIG_H
#define VECTOR_SLICER_FILLINGCONFIG_H

#include <string>
#include <vector>
#include <boost/filesystem/path.hpp>

namespace fs = boost::filesystem;


enum fillingMethod {
    ConsecutivePerimeter, RandomPerimeter, ConsecutiveRadial, RandomRadial
};
enum configOptions {
    InitialFillingMethod, CollisionRadius, StepLength, PrintRadius, Repulsion, StartingPointSeparation, Seed, RepulsionRadius
};


class FillingConfig {
    fillingMethod filling_method;
    double collision_radius;
    double repulsion;
    int step_length;
    double starting_point_separation;
    double print_radius;
    unsigned int seed;
    double repulsion_radius;

    void readLineOfConfig(std::vector<std::string> line);

public:
    void printConfig();

    void setConfigOption(const configOptions &option, const std::string &value);

    fillingMethod getInitialFillingMethod() const;

    double getCollisionRadius() const;

    double getRepulsion() const;

    double getStartingPointSeparation() const;

    double getPrintRadius() const;

    unsigned int getSeed() const;

    double getRepulsionRadius() const;

    explicit FillingConfig();

    explicit FillingConfig(const fs::path &config_path);

    FillingConfig(fillingMethod new_perimeter_filling_method, int new_collision_radius,
                  int new_starting_point_separation, double new_repulsion, int new_step_length,
                  int new_print_radius, double new_repulsion_radius, unsigned int new_seed);


    void exportConfig(const fs::path &directory);

    int getStepLength() const;

    std::string getConfigOption(configOptions option);
};

bool areFillingConfigsTheSame(FillingConfig &first_config, FillingConfig &second_config);

configOptions stringToConfig(const std::string &string_option);

#endif //VECTOR_SLICER_FILLINGCONFIG_H
