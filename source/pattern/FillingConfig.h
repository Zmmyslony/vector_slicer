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
// You should have received a copy of the GNU General Public License along with Vector Slicer. If not, see <https://www.gnu.org/licenses/>.

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
    Perimeter, Dual
};
enum configOptions {
    InitialFillingMethod,
    CollisionRadius,
    StepLength,
    PrintRadius,
    Repulsion,
    StartingPointSeparation,
    Seed,
    RepulsionRadius
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

    [[nodiscard]] fillingMethod getInitialFillingMethod() const;

    [[nodiscard]] double getCollisionRadius() const;

    [[nodiscard]] double getRepulsion() const;

    [[nodiscard]] double getStartingPointSeparation() const;

    [[nodiscard]] double getPrintRadius() const;

    [[nodiscard]] unsigned int getSeed() const;

    [[nodiscard]] double getRepulsionRadius() const;

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

#endif //VECTOR_SLICER_FILLINGCONFIG_H
