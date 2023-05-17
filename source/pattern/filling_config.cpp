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

#include "filling_config.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

#include <unordered_map>
#include <iomanip>
#include <boost/filesystem/path.hpp>


void FillingConfig::printConfig() {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    stream << std::endl;
    stream << "\tCurrent configuration:" << std::endl;

    stream << "\t\tPerimeter filling is ";
    switch (filling_method) {
        case Perimeter:
            stream << "perimeter.";
            break;
        case Dual:
            stream << "dual.";
            break;
    }
    stream << std::endl;

    stream << "\t\tCollision radius is " << collision_radius << "." << std::endl;
    stream << "\t\tRepulsion is " << repulsion << "." << std::endl;
    stream << "\t\tRepulsionRadius  is " << repulsion_radius << "." << std::endl;
    stream << "\t\tPrint radius is " << print_radius << "." << std::endl;
    stream << "\t\tStarting point separation is " << starting_point_separation << "." << std::endl;
    stream << "\t\tSeed is " << seed << "." << std::endl;
    stream << "\t\tStep length is " << step_length << "." << std::endl;

    std::string message = stream.str();
    std::cout << message << std::endl;
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
        case RepulsionRadius:
            return std::to_string(repulsion_radius);
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
            {"Seed",                    configOptions::Seed},
            {"RepulsionRadius",         configOptions::RepulsionRadius}
    };
    auto it = mapping.find(string_option);
    if (it != mapping.end()) {
        return it->second;
    } else {
        throw std::runtime_error("Unrecognised config option: " + string_option);
    }
}


fillingMethod stringToMethod(const std::string &string_option) {
    static std::unordered_map<std::string, fillingMethod> const mapping = {
            {"Perimeter", fillingMethod::Perimeter},
            {"Dual",      fillingMethod::Dual}
    };
    auto it = mapping.find(string_option);
    if (it != mapping.end()) {
        return it->second;
    } else {
        throw std::runtime_error("Unrecognised filling method: " + string_option);
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
            } else collision_radius = 0;
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
        case RepulsionRadius:
            repulsion_radius = std::stod(value);
            break;
    }
}


void FillingConfig::readLineOfConfig(std::vector<std::string> line) {

    std::string parameter_name = line[0];
    std::string value = line[1];
//    if (line.size() > 2 && parameter_name == "seed") {
//
//    }
    try {
        configOptions option = stringToConfig(parameter_name);
        setConfigOption(option, value);
    }
    catch (const std::runtime_error &error_message) {
        std::cout << "Error occurred while trying to read the config: \n\t" << error_message.what() << std::endl;
        std::cout << "Ignoring this entry and trying to read the remaining lines of config." << std::endl;
    }
}


FillingConfig::FillingConfig(const fs::path &config_path) : FillingConfig() {
    std::string line;
    std::ifstream file(config_path.string());

    while (std::getline(file, line)) {
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        std::string element;
        std::stringstream line_stream(line);
        std::vector<std::string> row;

        while (std::getline(line_stream, element, ' ')) {
            row.push_back(element);
        }
        readLineOfConfig(row);
    }
}

std::vector<FillingConfig> readMultiSeedConfig(const fs::path &config_path) {
    FillingConfig base_config(config_path);

    std::vector<int> seed_list;
    std::string line;
    std::ifstream file(config_path.string());

    while (std::getline(file, line)) {
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        std::string element;
        std::stringstream line_stream(line);
        std::vector<std::string> row;

        while (std::getline(line_stream, element, ' ')) {
            row.push_back(element);
        }
        if (row[0] == "Seed") {
            for (int i = 1; i < row.size(); i++) {
                seed_list.emplace_back(std::stoi(row[i]));
            }
        }
    }

    std::vector <FillingConfig> filling_config_list;
    for (auto &seed: seed_list) {
        filling_config_list.emplace_back(FillingConfig(base_config, seed));
    }
    return filling_config_list;
}

FillingConfig::FillingConfig(const FillingConfig &source_config, int source_seed) : FillingConfig(source_config) {
    seed = source_seed;
}

FillingConfig::FillingConfig(fillingMethod new_perimeter_filling_method, int new_collision_radius,
                             int new_starting_point_separation, double new_repulsion, int new_step_length,
                             int new_print_radius, double new_repulsion_radius, unsigned int new_seed) {
    filling_method = new_perimeter_filling_method;
    collision_radius = new_collision_radius;
    starting_point_separation = new_starting_point_separation;
    repulsion = new_repulsion;
    step_length = new_step_length;
    print_radius = new_print_radius;
    repulsion_radius = new_repulsion_radius;
    seed = new_seed;
}

void exportConfigList(const std::vector<FillingConfig> &configs, fs::path path) {
    std::ofstream file(path.string());

    if (file.is_open()) {
        file << "InitialFillingMethod ";
        switch (configs[0].getInitialFillingMethod()) {
            case Perimeter:
                file << "Perimeter";
                break;
            case Dual:
                file << "Dual";
                break;
        }
        file << std::endl;
        file << "CollisionRadius " << configs[0].getCollisionRadius() << std::endl;
        file << "StartingPointSeparation " << configs[0].getStartingPointSeparation() << std::endl;
        file << "Repulsion " << configs[0].getRepulsion() << std::endl;
        file << "RepulsionRadius " << configs[0].getRepulsionRadius() << std::endl;
        file << "StepLength " << configs[0].getStepLength() << std::endl;
        file << "PrintRadius " << configs[0].getPrintRadius() << std::endl;
        file << "Seed ";
        for (auto &config: configs) {
            file << config.getSeed() << " ";
        }
        file << std::endl;
        file.close();
    }
}

void FillingConfig::exportConfig(const fs::path &directory, const std::string &suffix) {
    exportConfigList({*this}, directory);
}

FillingConfig::FillingConfig() : FillingConfig(Perimeter, 5,
                                               5, 0, 10,
                                               5, 0, 0) {}

double FillingConfig::getRepulsionRadius() const {
    return repulsion_radius;
}


bool isConfigOptionTheSame(configOptions option, FillingConfig &first_config, FillingConfig &second_config) {
    return first_config.getConfigOption(option) == second_config.getConfigOption(option);
}