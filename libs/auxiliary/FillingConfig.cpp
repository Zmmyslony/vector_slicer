//
// Created by zmmys on 05/11/2021.
//

#include "FillingConfig.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>

void FillingConfig::printConfig() {
    std::string message;
    message += "Current configuration:";

    message += "\n\tRandom perimeter filling is ";
    if (isPerimeterFilledRandomly) {
        message += "ON.";
    }
    else {
        message += "OFF.";
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

void FillingConfig::setPerimeterFillingToRandom() {
    isPerimeterFilledRandomly = true;
}

void FillingConfig::setPerimeterFillingToConsecutive() {
    isPerimeterFilledRandomly = false;
}

void FillingConfig::setRepulsion(double repulsionCoefficient) {
    repulsion = repulsionCoefficient;
}

void FillingConfig::setStepLength(int step) {
    stepLength = step;
}

void FillingConfig::setPrintRadius(int radius) {
    printRadius = radius;
}

bool FillingConfig::getPerimeterFillingMode() const {
    return isPerimeterFilledRandomly;
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

int FillingConfig::getPrintRadius() const {
    return printRadius;
}


void FillingConfig::readLineOfConfig(std::vector<std::string> line) {
    std::string parameterName = line[0];
    std::transform(parameterName.begin(), parameterName.end(), parameterName.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    std::string value = line[1];

    if (parameterName == "isfillingmoderandom") {
        if (std::stoi(value) == 1) {
            setPerimeterFillingToRandom();
        }
        else if (std::stoi(value) == 0) {
            setPerimeterFillingToConsecutive();
        }
    }
    else if (parameterName == "collisionradius") {
        setCollisionRadius(std::stoi(value));
    }
    else if (parameterName == "steplength") {
        setStepLength(std::stoi(value));
    }
    else if (parameterName == "printradius") {
        setPrintRadius(std::stoi(value));
    }
    else if (parameterName == "repulsion") {
        setRepulsion(std::stod(value));
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