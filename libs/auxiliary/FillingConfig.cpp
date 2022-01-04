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


void FillingConfig::printConfig() {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);

    stream << "\tCurrent configuration:" << std::endl;

    stream << "\t\tPerimeter filling is ";
    switch (fillingMethod) {
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

    stream << "\t\tCollision radius is " << collisionRadius << "." << std::endl;
    stream << "\t\tRepulsion is " << repulsion << "." << std::endl;
    stream << "\t\tPrint radius is " << printRadius << "." << std::endl;
    stream << "\t\tStarting point separation is " << startingPointSeparation << "." << std::endl;
    stream << "\t\tSeed is " << seed << "." << std::endl;
    stream << "\t\tStep length is " << stepLength << "." << std::endl;

    std::string message = stream.str();
    std::cout << message;
}

FillingMethod FillingConfig::getInitialFillingMethod() const {
    return fillingMethod;
}

double FillingConfig::getCollisionRadius() const {
    return collisionRadius;
}

double FillingConfig::getRepulsion() const {
    return repulsion;
}

int FillingConfig::getStepLength() const {
    return stepLength;
}

double FillingConfig::getStartingPointSeparation() const {
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

double FillingConfig::getPrintRadius() const {
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
            collisionRadius = std::stod(value);
            break;
        case StepLength:
            stepLength = std::stoi(value);
            break;
        case PrintRadius:
            printRadius = std::stod(value);
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
