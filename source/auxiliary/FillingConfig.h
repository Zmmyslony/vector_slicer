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

enum FillingMethod {
    ConsecutivePerimeter, RandomPerimeter, ConsecutiveRadial, RandomRadial
};
enum ConfigOptions {
    InitialFillingMethod, CollisionRadius, StepLength, PrintRadius, Repulsion, StartingPointSeparation, Seed
};


class FillingConfig {
    FillingMethod fillingMethod;
    double collisionRadius;
    double repulsion;
    int stepLength;
    double startingPointSeparation;
    double printRadius;
    unsigned int seed;

    void readLineOfConfig(std::vector<std::string> line);

public:
    void printConfig();

    void setConfigOption(const ConfigOptions &option, const std::string &value);

    FillingMethod getInitialFillingMethod() const;

    double getCollisionRadius() const;

    double getRepulsion() const;

    int getStepLength() const;

    double getStartingPointSeparation() const;

    double getPrintRadius() const;

    unsigned int getSeed() const;


    explicit FillingConfig(std::string &configPath);

    FillingConfig(FillingMethod newPerimeterFillingMethod, int newCollisionRadius,
                  int newStartPointSeparation, double newRepulsion, int newStepLength,
                  int newPrintRadius, unsigned int seed);


    std::string getConfigOption(ConfigOptions option);

    void exportConfig(const std::string &directory);
};

bool areFillingConfigsTheSame(FillingConfig &firstConfig, FillingConfig &secondConfig);

ConfigOptions stringToConfig(const std::string &stringOption);

#endif //VECTOR_SLICER_FILLINGCONFIG_H
