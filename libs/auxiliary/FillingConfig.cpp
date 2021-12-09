//
// Created by zmmys on 05/11/2021.
//

#include "FillingConfig.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <sstream>


void FillingConfig::printConfig() {
    std::string message;
    message += "Current configuration:";

    message += "\n\tPerimeter filling is ";
    switch (fillingMethod) {
        case ConsecutivePerimeter:
            message += "consecutive perimeter.";
            break;
        case RandomPerimeter:
            message += "random perimeter.";
            break;
        case ConsecutiveRadial:
            message += "consecutive radial.";
            break;
        case RandomRadial:
            message += "random radial.";
            break;
    }

    message += "\n\tCollision radius is ";
    message += std::to_string(collisionRadius);
    message += ".";

    message += "\n\tRepulsion is ";
    message += std::to_string(repulsion);
    message += ".";

    message += "\n\tPrint radius is ";
    message += std::to_string(printRadius);
    message += ".";

    message += "\n\tStarting point separation is ";
    message += std::to_string(startingPointSeparation);
    message += ".";

    message += "\n\tSeed is ";
    message += std::to_string(seed);
    message += ".";

    message += "\n\tStep length is ";
    message += std::to_string(stepLength);
    message += ".\n";
    std::cout << message;
}

FillingMethod FillingConfig::getInitialFillingMethod() const {
    return fillingMethod;
}

int FillingConfig::getCollisionRadius() const {
    return collisionRadius;
}

double FillingConfig::getRepulsion() const {
    return repulsion;
}

int FillingConfig::getStepLength() const {
    return stepLength;
}

int FillingConfig::getStartingPointSeparation() const {
    return startingPointSeparation;
}

std::string FillingConfig::getConfigOption(ConfigOptions option) {
    switch (option) {
        case InitialFillingMethod:
            return std::to_string(fillingMethod);
        case CollisionRadius:
            return std::to_string(collisionRadius);
        case StepLength:
            return std::to_string(stepLength);
        case PrintRadius:
            return std::to_string(printRadius);
        case Repulsion:
            return std::to_string(repulsion);
        case StartingPointSeparation:
            return std::to_string(startingPointSeparation);
        case Seed:
            return std::to_string(seed);
    }
}

unsigned int FillingConfig::getSeed() const {
    return seed;
}

int FillingConfig::getPrintRadius() const {
    return printRadius;
}

ConfigOptions stringToConfig(const std::string &stringOption) {
    static std::unordered_map<std::string, ConfigOptions> const mapping = {
            {"InitialFillingMethod",    ConfigOptions::InitialFillingMethod},
            {"CollisionRadius",         ConfigOptions::CollisionRadius},
            {"StepLength",              ConfigOptions::StepLength},
            {"PrintRadius",             ConfigOptions::PrintRadius},
            {"Repulsion",               ConfigOptions::Repulsion},
            {"StartingPointSeparation", ConfigOptions::StartingPointSeparation},
            {"Seed",                    ConfigOptions::Seed}
    };
    auto it = mapping.find(stringOption);
    if (it != mapping.end()) {
        return it->second;
    }
}


FillingMethod stringToMethod(const std::string &stringOption) {
    static std::unordered_map<std::string, FillingMethod> const mapping = {
            {"ConsecutivePerimeter", FillingMethod::ConsecutivePerimeter},
            {"RandomPerimeter",      FillingMethod::RandomPerimeter},
            {"ConsecutiveRadial",    FillingMethod::ConsecutiveRadial},
            {"RandomRadial",         FillingMethod::RandomRadial}
    };
    auto it = mapping.find(stringOption);
    if (it != mapping.end()) {
        return it->second;
    }
}


void FillingConfig::setConfigOption(const ConfigOptions &option, const std::string &value) {
    switch (option) {
        case InitialFillingMethod:
            fillingMethod = stringToMethod(value);
            break;
        case CollisionRadius:
            collisionRadius = std::stoi(value);
            break;
        case StepLength:
            stepLength = std::stoi(value);
            break;
        case PrintRadius:
            printRadius = std::stoi(value);
            break;
        case Repulsion:
            repulsion = std::stod(value);
            break;
        case StartingPointSeparation:
            startingPointSeparation = std::stoi(value);
            break;
        case Seed:
            seed = std::stoi(value);
            break;
    }
}


void FillingConfig::readLineOfConfig(std::vector<std::string> line) {

    std::string parameterName = line[0];
    std::string value = line[1];
    ConfigOptions option = stringToConfig(parameterName);
    setConfigOption(option, value);
}


FillingConfig::FillingConfig(std::string &configPath) {
    std::string line;
    std::ifstream file(configPath);

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

FillingConfig::FillingConfig(FillingMethod newPerimeterFillingMethod,
                             int newCollisionRadius, int newStartingPointSeparation,
                             double newRepulsion, int newStepLength, int newPrintRadius, unsigned int newSeed) {
    fillingMethod = newPerimeterFillingMethod;
    collisionRadius = newCollisionRadius;
    startingPointSeparation = newStartingPointSeparation;
    repulsion = newRepulsion;
    stepLength = newStepLength;
    printRadius = newPrintRadius;
    seed = newSeed;
}
