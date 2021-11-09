//
// Created by zmmys on 05/11/2021.
//

#include "FillingConfig.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <sstream>


void FillingConfig::printConfig() {
    std::string message;
    message += "Current configuration:";

    message += "\n\tRandom perimeter filling is ";
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

    message += "\n\tCollision radius is";
    message += std::to_string(collisionRadius);
    message += ".";

    message += "\n\tRepulsion is ";
    message += std::to_string(repulsion);
    message += ".";
    std::cout << message;
}

void FillingConfig::setCollisionRadius(int radius) {
    collisionRadius = radius;
}

void FillingConfig::setInitialFillingMethod(FillingMethod method) {
    fillingMethod = method;
}

void FillingConfig::setInitialFillingMethodToRandomPerimeter() {
    fillingMethod = RandomPerimeter;
}

void FillingConfig::setInitialFillingMethodToConsecutivePerimeter() {
    fillingMethod = ConsecutivePerimeter;
}

void FillingConfig::setInitialFillingMethodToRandomRadial() {
    fillingMethod = RandomRadial;
}

void FillingConfig::setInitialFillingMethodToConsecutiveRadial() {
    fillingMethod = ConsecutiveRadial;
}

void FillingConfig::setRepulsion(double repulsionCoefficient) {
    repulsion = repulsionCoefficient;
}

void FillingConfig::setStepLength(int step) {
    stepLength = step;
}

void FillingConfig::setMinimalStepLength(int step) {
    minimalStepLength = step;
}

void FillingConfig::setPrintRadius(int radius) {
    printRadius = radius;
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

int FillingConfig::getMinimalStepLength() const {
    return minimalStepLength;
}

int FillingConfig::getPrintRadius() const {
    return printRadius;
}

ConfigOptions stringToConfig(std::string stringOption) {
    static std::unordered_map<std::string, ConfigOptions> const mapping = {
            {"InitialFillingMethod",   ConfigOptions::InitialFillingMethod},
            {"IsInitialFillingRandom", ConfigOptions::IsInitialFillingRandom},
            {"CollisionRadius",        ConfigOptions::CollisionRadius},
            {"StepLength",             ConfigOptions::StepLength},
            {"PrintRadius",            ConfigOptions::PrintRadius},
            {"Repulsion",              ConfigOptions::Repulsion},
            {"MinimalStepLength",      ConfigOptions::MinimalStepLength}
    };
    auto it = mapping.find(stringOption);
    if (it != mapping.end()) {
        return it->second;
    }
}

FillingMethod stringToMethod(std::string stringOption) {
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


void FillingConfig::readLineOfConfig(std::vector<std::string> line) {
    std::string parameterName = line[0];
    std::transform(parameterName.begin(), parameterName.end(), parameterName.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    std::string value = line[1];

    ConfigOptions option = stringToConfig(parameterName);

    switch (option) {
        case InitialFillingMethod:
            setInitialFillingMethod(stringToMethod(value));
            break;
        case CollisionRadius:
            setCollisionRadius(std::stoi(value));
            break;
        case StepLength:
            setStepLength(std::stoi(value));
            break;
        case PrintRadius:
            setPrintRadius(std::stoi(value));
            break;
        case Repulsion:
            setRepulsion(std::stod(value));
            break;
        case MinimalStepLength:
            setMinimalStepLength(std::stoi(value));
            break;
    }
}


FillingConfig::FillingConfig(std::string &configPath) {
    std::string line;
    std::ifstream file(configPath);

    while (std::getline(file, line)) {
        std::string element;
        std::stringstream line_stream(line);
        std::vector<std::string> row;

        while (std::getline(line_stream, element, ',')) {
            row.push_back(element);
        }
        readLineOfConfig(row);
    }
}

FillingConfig::FillingConfig(FillingMethod newPerimeterFillingMethod,
                             int newCollisionRadius, int newMinimalStepLength,
                             double newRepulsion, int newStepLength, int newPrintRadius) {
    fillingMethod = newPerimeterFillingMethod;
    collisionRadius = newCollisionRadius;
    minimalStepLength = newMinimalStepLength;
    repulsion = newRepulsion;
    stepLength = newStepLength;
    printRadius = newPrintRadius;
}

