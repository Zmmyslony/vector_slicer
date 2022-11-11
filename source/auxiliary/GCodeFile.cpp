//
// Created by Michał Zmyślony on 04/01/2022.
//

#include <fstream>
#include "GCodeFile.h"
#include "ValarrayOperations.h"
#include "Exporting.h"
#include <iostream>

const double VERTICAL_MOVE_SLOWDOWN = 3;
const std::valarray<double> ZERO_POSITION = {1, 1};

void GCodeFile::generalCommand(int number, const std::string &suffix) {
    std::stringstream commandNumber;
    commandNumber << std::setw(2) << std::setfill('0') << number;
    bodyStream << "G" << commandNumber.str() << " " << suffix + "\n";
}

void GCodeFile::generalCommand(const std::vector<char> &commands, const std::vector<bool> &isInt,
                               const std::vector<double> &values) {
    if (commands.size() == values.size() && commands.size() == isInt.size()) {
        for (int i = 0; i < commands.size(); i++) {
            if (isInt[i]) {
                bodyStream << commands[i] << int(values[i]) << " ";
            } else {
                bodyStream << commands[i] << std::fixed << values[i] << " ";
            }
        }
        bodyStream << "\n";
    }
    else {
        addComment("\t\tSYNTAX ERROR");
    }
}

void GCodeFile::generalCommand(const std::vector<char> &commands, const std::vector<bool> &isInt,
                               const std::vector<double> &values, const std::string& comment) {
    if (commands.size() == values.size() && commands.size() == isInt.size()) {
        for (int i = 0; i < commands.size(); i++) {
            if (isInt[i]) {
                bodyStream << commands[i] << int(values[i]) << " ";
            } else {
                bodyStream << commands[i] << std::fixed << values[i] << " ";
            }
        }
        bodyStream << "\t;" << comment << "\n";
    }
}

void GCodeFile::generalCommand(const char &command, double value) {
    generalCommand({command}, {false}, {value});
}

void GCodeFile::generalCommand(const char &command, int value) {
    generalCommand({command}, {true}, {(double) value});
}

void GCodeFile::setRelativePositioning() {
//    addComment("Setting relative positioning");
    generalCommand('G', 91);
}

void GCodeFile::setAbsolutePositioning() {
//    addComment("Setting absolute positioning");
    generalCommand('G', 90);
}

void GCodeFile::autoHome() {
    addComment("Auto homing");
    setRelativePositioning();
    moveVertical(10);
    setAbsolutePositioning();
    generalCommand('G', 28);
    positions = {0, 0, 0};
}

void GCodeFile::levelBed() {
    addComment("Levelling the bed");
    generalCommand('G', 29);
}

void GCodeFile::setTemperatureHotend(int temperature) {
    addComment("Setting hotend temperature");
    generalCommand({'M', 'S'}, {true, true}, {109, (double) temperature});
}

void GCodeFile::setTemperatureHotendGradual(int temperature) {
    for (int currentTemperature = 20; currentTemperature < temperature; currentTemperature++) {
        setTemperatureHotend(currentTemperature);
    }
}

void GCodeFile::setTemperatureBed(int temperature) {
    addComment("Setting bed temperature");
    generalCommand({'M', 'S'}, {true, true}, {190, (double) temperature});
}

void GCodeFile::turnMotorsOff() {
    addComment("Turning motors off");
    generalCommand('M', 84);
}

void GCodeFile::movePlanar(const std::valarray<double> &xy) {
    generalCommand({'G', 'X', 'Y', 'F'}, {true, false, false, true}, {0, xy[0], xy[1], (double) moveSpeed});
    positions[0] = xy[0];
    positions[1] = xy[1];
}

void GCodeFile::moveVertical(double z) {
    generalCommand({'G', 'Z', 'F'}, {true, false, true}, {0, z, (double) moveSpeed / VERTICAL_MOVE_SLOWDOWN});
    positions[2] = z;
}

void GCodeFile::moveVerticalRelative(double deltaZ) {
    positions[2] += deltaZ;
    bodyStream << "G0 Z" << positions[2] << " F" << moveSpeed / VERTICAL_MOVE_SLOWDOWN << "\n";
}

void GCodeFile::move(double x, double y, double z) {
    generalCommand({'G', 'X', 'Y', 'Z', 'F'}, {true, false, false, false, true}, {0, x, y, z, (double) moveSpeed});
    positions = {x, y, z};
}

void GCodeFile::extrude(const std::valarray<double> &xy) {
    std::valarray<double> newPositions = {xy[0], xy[1], positions[2]};
    double extrusion = extrusionCoefficient * norm(newPositions - positions);
    extrusionValue += extrusion;
    positions = newPositions;
    bodyStream << "G1 X" << xy[0] << " Y" << xy[1] << " F" << printSpeed << " E" << extrusionValue << "\n";
}



void GCodeFile::setCurrentCoordinatesToZero() {
    addComment("Setting current coordinates as new zero");
    generalCommand({'G', 'X', 'Y', 'Z'}, {true, false, false, false}, {92, 0, 0, 0});
}

void GCodeFile::resetPositionOfFilament() {
    bodyStream << "G92 E0" << "\n";
}

void GCodeFile::addComment(const std::string &comment) {
    bodyStream << "; " << comment << "\n";
}


GCodeFile::GCodeFile(int moveSpeed, int printSpeed, double extrusionCoefficient) :
        moveSpeed(moveSpeed),
        printSpeed(printSpeed),
        extrusionCoefficient(extrusionCoefficient) {
    bodyStream.precision(3);
}

GCodeFile::GCodeFile() :
        GCodeFile(600, 100, 1) {
}


void GCodeFile::init(int hotendTemperature, int bedTemperature, double cleanLength) {
    autoHome();
    levelBed();

    setAbsolutePositioning();
    resetPositionOfFilament();
    setTemperatureBed(bedTemperature);
    setTemperatureHotendGradual(hotendTemperature);

    move(1, 1, 0);
    extrude(ZERO_POSITION + std::valarray<double>({cleanLength, 0}));
}

void GCodeFile::init(int hotendTemperature) {
    init(hotendTemperature, 0, 20);
}

void GCodeFile::shutDown() {
    setTemperatureBed(0);
    setTemperatureHotend(0);

    moveVertical(10);
    movePlanar(ZERO_POSITION);
    turnMotorsOff();
}

std::string GCodeFile::getText() {
    return bodyStream.str();
}

void GCodeFile::printPath(const std::vector<std::valarray<int>>& path, const std::valarray<double> &positionOffset,
                          double gridDistance) {

    addComment("Moving up.");
    moveVerticalRelative(1);
    addComment("Moving to new starting point.");

    movePlanar(itodArray(path[0]) * gridDistance + positionOffset);
    addComment("Moving down.");
    moveVerticalRelative(-1);
    addComment("Starting new path.");
    for (auto &position: path) {
        extrude(itodArray(position) * gridDistance + positionOffset);
    }
}

void
GCodeFile::printPattern(const std::vector<std::vector<std::valarray<int>>> &sortedSequenceOfPaths,
                        const std::valarray<double> &positionOffset,
                        double gridDistance) {
    for (auto &path: sortedSequenceOfPaths) {
        printPath(path, positionOffset, gridDistance);
    }
}

void GCodeFile::exportToFile(const std::string &path) {
    std::string filename = path + R"(\results\pattern.gcode)";
    std::ofstream file(filename);

    file << getText();
    file.close();
}

void GCodeFile::addBreak() {
    bodyStream << "\n";
}


void generateGCode(const std::string &baseDirectory, int temperature, double cleaningDistance,
                   const std::valarray<double> &positionOffset, double gridDistance) {
    std::cout << std::endl;
    std::string directoryPath = baseDirectory + R"(\results)";
    std::vector<std::vector<std::valarray<int>>> sortedPaths = read3DVectorFromFile(directoryPath, "best_paths");
    GCodeFile gCodeFile;
    gCodeFile.init(temperature, 0, cleaningDistance);
    gCodeFile.printPattern(sortedPaths, positionOffset, gridDistance);
    gCodeFile.shutDown();

    gCodeFile.exportToFile(baseDirectory);
}
