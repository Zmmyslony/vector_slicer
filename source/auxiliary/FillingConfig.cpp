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

#include "FillingConfig.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <sstream>
#include <iomanip>
#include <boost/filesystem/path.hpp>


void FillingConfig::printConfig() {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);

    stream << "\tCurrent configuration:" << std::endl;

    stream << "\t\tPerimeter filling is ";
    switch (filling_method) {
        case ConsecutivePerimeter:
            stream << "consecutive perimeter.";
            break;
        case RandomPerimeter:
            stream << "random perimeter.";
            break;
        case ConsecutiveRadial:
            stream << "consecutive radial.";
            break;
        case RandomRadial:
            stream << "random radial.";
            break;
    }
    stream << std::endl;

    stream << "\t\tCollision radius is " << collision_radius << "." << std::endl;
    stream << "\t\tRepulsion is " << repulsion << "." << std::endl;
    stream << "\t\tPrint radius is " << print_radius << "." << std::endl;
    stream << "\t\tStarting point separation is " << starting_point_separation << "." << std::endl;
    stream << "\t\tSeed is " << seed << "." << std::endl;
    stream << "\t\tStep length is " << step_length << "." << std::endl;

    std::string message = stream.str();
    std::cout << message;
}

fillingMethod FillingConfig::getInitialFillingMethod() const {
    return filling_method;
}

double FillingConfig::getCollisionRadius() const {
    return collision_radius;
}

double FillingConfig::getRepulsion() const {
    return repulsion;
}

int FillingConfig::getStepLength() const {
    return step_length;
}

double FillingConfig::getStartingPointSeparation() const {
    return starting_point_separation;
}

std::string FillingConfig::getConfigOption(configOptions option) {
    switch (option) {
        case InitialFillingMethod:
            return std::to_string(filling_method);
        case CollisionRadius:
            return std::to_string(collision_radius);
        case StepLength:
            return std::to_string(step_length);
        case PrintRadius:
            return std::to_string(print_radius);
        case Repulsion:
            return std::to_string(repulsion);
        case StartingPointSeparation:
            return std::to_string(starting_point_separation);
        case Seed:
            return std::to_string(seed);
    }
}

unsigned int FillingConfig::getSeed() const {
    return seed;
}

double FillingConfig::getPrintRadius() const {
    return print_radius;
}

configOptions stringToConfig(const std::string &string_option) {
    static std::unordered_map<std::string, configOptions> const mapping = {
            {"InitialFillingMethod",    configOptions::InitialFillingMethod},
            {"CollisionRadius",         configOptions::CollisionRadius},
            {"StepLength",              configOptions::StepLength},
            {"PrintRadius",             configOptions::PrintRadius},
            {"Repulsion",               configOptions::Repulsion},
            {"StartingPointSeparation", configOptions::StartingPointSeparation},
            {"Seed",                    configOptions::Seed}
    };
    auto it = mapping.find(string_option);
    if (it != mapping.end()) {
        return it->second;
    }
    else {
        std::cout << "Unrecognised ConfigOption: " << string_option << std::endl;
    }
}


fillingMethod stringToMethod(const std::string &string_option) {
    static std::unordered_map<std::string, fillingMethod> const mapping = {
            {"ConsecutivePerimeter", fillingMethod::ConsecutivePerimeter},
            {"RandomPerimeter",      fillingMethod::RandomPerimeter},
            {"ConsecutiveRadial",    fillingMethod::ConsecutiveRadial},
            {"RandomRadial",         fillingMethod::RandomRadial}
    };
    auto it = mapping.find(string_option);
    if (it != mapping.end()) {
        return it->second;
    }
    else {
        std::cout << "Unrecognised filling_method:" << string_option << std::endl;
    }
}


void FillingConfig::setConfigOption(const configOptions &option, const std::string &value) {
    switch (option) {
        case InitialFillingMethod:
            filling_method = stringToMethod(value);
            break;
        case CollisionRadius:
            if (std::stod(value) > 0) {
                collision_radius = std::stod(value);
            }
            else collision_radius = 0;
            break;
        case StepLength:
            step_length = std::stoi(value);
            break;
        case PrintRadius:
            print_radius = std::stod(value);
            break;
        case Repulsion:
            repulsion = std::stod(value);
            break;
        case StartingPointSeparation:
            starting_point_separation = std::stoi(value);
            break;
        case Seed:
            seed = std::stoi(value);
            break;
    }
}


void FillingConfig::readLineOfConfig(std::vector<std::string> line) {

    std::string parameter_name = line[0];
    std::string value = line[1];
    configOptions option = stringToConfig(parameter_name);
    setConfigOption(option, value);
}


FillingConfig::FillingConfig(fs::path &config_path) {
    std::string line;
    std::ifstream file(config_path.string());

    while (std::getline(file, line)) {
        std::string element;
        std::stringstream line_stream(line);
        std::vector<std::string> row;

        while (std::getline(line_stream, element, ' ')) {
            row.push_back(element);
        }
        readLineOfConfig(row);
    }
}


FillingConfig::FillingConfig(fillingMethod new_perimeter_filling_method,
                             int new_collision_radius, int new_starting_point_separation,
                             double new_repulsion, int new_step_length, int new_print_radius, unsigned int new_seed) {
    filling_method = new_perimeter_filling_method;
    collision_radius = new_collision_radius;
    starting_point_separation = new_starting_point_separation;
    repulsion = new_repulsion;
    step_length = new_step_length;
    print_radius = new_print_radius;
    seed = new_seed;
}

void FillingConfig::exportConfig(const fs::path &directory) {
    fs::path filename = directory / "results" / "best_config.txt";
    std::ofstream file(filename.string());

    if (file.is_open()) {
        file << "InitialFillingMethod ";
        switch (filling_method) {
            case ConsecutivePerimeter:
                file << "ConsecutivePerimeter";
                break;
            case RandomPerimeter:
                file << "RandomPerimeter";
                break;
            case ConsecutiveRadial:
                file << "ConsecutiveRadial";
                break;
            case RandomRadial:
                file << "RandomRadial";
                break;
        }
        file << std::endl;
        file << "CollisionRadius " << collision_radius << std::endl;
        file << "StartingPointSeparation " << starting_point_separation << std::endl;
        file << "Repulsion " << repulsion << std::endl;
        file << "StepLength " << step_length << std::endl;
        file << "PrintRadius " << print_radius << std::endl;
        file << "Seed " << seed << std::endl;
        file.close();
    }
}


bool isConfigOptionTheSame(configOptions option, FillingConfig &first_config, FillingConfig &second_config) {
    return first_config.getConfigOption(option) == second_config.getConfigOption(option);
}


bool areFillingConfigsTheSame(FillingConfig &first_config, FillingConfig &second_config) {
    bool repulsion = isConfigOptionTheSame(Repulsion, first_config, second_config);
    bool collision_radius = isConfigOptionTheSame(CollisionRadius, first_config, second_config);
    bool starting_point_separation = isConfigOptionTheSame(StartingPointSeparation, first_config, second_config);
    bool step_length = isConfigOptionTheSame(StepLength, first_config, second_config);
    bool print_radius = isConfigOptionTheSame(PrintRadius, first_config, second_config);

    if (repulsion && collision_radius && starting_point_separation && step_length && print_radius) {
        return true;
    }
    else {
        return false;
    }
}