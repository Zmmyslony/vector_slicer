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
                bodyStream << commands[i] << std::to_string(int(values[i])) << " ";
            } else {
                bodyStream << commands[i] << std::to_string(values[i]) << " ";
            }
        }
        bodyStream << "\n";
    }
}

void GCodeFile::generalCommand(const char &command, double value) {
    generalCommand({command}, {false}, {value});
}

void GCodeFile::generalCommand(const char &command, int value) {
    generalCommand({command}, {true}, {(double) value});
}

void GCodeFile::setRelativePositioning() {
//    bodyStream << "G91\n";
    generalCommand('G', 91);
}

void GCodeFile::setAbsolutePositioning() {
//    bodyStream << "G90\n";
    generalCommand('G', 90);
}

void GCodeFile::autoHome() {
//    bodyStream << "G28\n";
    generalCommand('G', 28);
    positions = {0, 0, 0};
}

void GCodeFile::levelBed() {
//    bodyStream << "G29\n";
    generalCommand('G', 29);
}

void GCodeFile::setTemperatureHotend(int temperature) {
//    bodyStream << "M109 S" << temperature << "\n";
    generalCommand({'M', 'S'}, {true, true}, {109, (double) temperature});
}

void GCodeFile::setTemperatureHotendGradual(int temperature) {
    for (int currentTemperature = 20; currentTemperature < temperature; currentTemperature++) {
        setTemperatureHotend(currentTemperature);
    }
}

void GCodeFile::setTemperatureBed(int temperature) {
//    bodyStream << "M190 S" << temperature << "\n";
    generalCommand({'M', 'S'}, {true, true}, {190, (double) temperature});
}

void GCodeFile::turnMotorsOff() {
//    bodyStream << "M84\n";
    generalCommand('M', 84);
}

void GCodeFile::movePlanar(const std::valarray<double> &xy) {
//    bodyStream << "G0 X" << xy[0] << " Y" << xy[1] << " F" << moveSpeed << "\n";
    generalCommand({'G', 'X', 'Y', 'F'}, {true, false, false, true}, {0, xy[0], xy[1], (double) moveSpeed});
    positions[0] = xy[0];
    positions[1] = xy[1];
}

void GCodeFile::moveVertical(double z) {
//    bodyStream << "G0 Z" << z << " F" << moveSpeed / VERTICAL_MOVE_SLOWDOWN << "\n";
    generalCommand({'G', 'Z', 'F'}, {true, false, true}, {0, z, (double) moveSpeed / VERTICAL_MOVE_SLOWDOWN});
    positions[2] = z;
}

void GCodeFile::moveVerticalRelative(double deltaZ) {
    positions[2] += deltaZ;
    bodyStream << "G0 Z" << positions[2] << " F" << moveSpeed / VERTICAL_MOVE_SLOWDOWN << "\n";
}

void GCodeFile::move(double x, double y, double z) {
//    bodyStream << "G0 X" << x << " Y" << y << " Z" << z << " F" << moveSpeed << "\n";
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

void GCodeFile::printPath(const std::vector<std::valarray<int>> path, const std::valarray<double> &positionOffset,
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
