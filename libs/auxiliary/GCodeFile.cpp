//
// Created by Michał Zmyślony on 04/01/2022.
//

#include "GCodeFile.h"
#include "ValarrayOperations.h"

const double VERTICAL_MOVE_SLOWDOWN = 3;
const std::valarray<double> ZERO_POSITION = {1, 1};

void GCodeFile::generalCommand(int number, const std::string &suffix) {
    std::stringstream commandNumber;
    commandNumber << std::setw(2) << std::setfill('0') << number;
    bodyStream << "G" << commandNumber.str() << " " << suffix + "\n";
}

void GCodeFile::setRelativePositioning() {
    bodyStream << "G91\n";
}

void GCodeFile::setAbsolutePositioning() {
    bodyStream << "G90\n";
}

void GCodeFile::autoHome() {
    bodyStream << "G28\n";
    positions = {0, 0, 0};
}

void GCodeFile::levelBed() {
    bodyStream << "G29\n";
}

void GCodeFile::setTemperatureHotend(int temperature) {
    bodyStream << "M109 S" << temperature << "\n";
}

void GCodeFile::setTemperatureHotendGradual(int temperature) {
    for (int currentTemperature = 20; currentTemperature < temperature; temperature++) {
        setTemperatureHotend(currentTemperature);
    }
}

void GCodeFile::setTemperatureBed(int temperature) {
    bodyStream << "M190 S" << temperature << "\n";
}

void GCodeFile::turnMotorsOff() {
    bodyStream << "M84\n";
}

void GCodeFile::movePlanar(const std::valarray<double> &xy) {
    bodyStream << "G0 X" << xy[0] << " Y" << xy[0] << " F" << moveSpeed;
    positions[0] = xy[0];
    positions[1] = xy[1];
}

void GCodeFile::moveVertical(double z) {
    bodyStream << "G0 Z" << z << " F" << moveSpeed / VERTICAL_MOVE_SLOWDOWN;
    positions[2] = z;
}

void GCodeFile::moveVerticalRelative(double deltaZ) {
    positions[2] += deltaZ;
    bodyStream << "G0 Z" << positions[2] << " F" << moveSpeed / VERTICAL_MOVE_SLOWDOWN;
}

void GCodeFile::move(double x, double y, double z) {
    bodyStream << "G0 X" << x << " Y" << y << " Z" << z << " F" << moveSpeed;
    positions = {x, y, z};
}

void GCodeFile::extrude(const std::valarray<double> &xy) {
    std::valarray<double> newPositions = {xy[0], xy[1], positions[2]};
    double extrusion = extrusionCoefficient * norm(newPositions - positions);
    extrusionValue += extrusion;
    positions = newPositions;
    bodyStream << "G1 X" << xy[0] << " Y" << xy[1] << " F" << printSpeed << " E" << extrusionValue;
}

void GCodeFile::resetPositionOfFilament() {
    bodyStream << "G92 0";
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

    moveVertical(20);
    movePlanar(ZERO_POSITION);
    turnMotorsOff();
}

std::string GCodeFile::getText() {
    return bodyStream.str();
}

void GCodeFile::printPath(const Path &path, const std::valarray<double> &positionOffset, double gridDistance) {
    moveVerticalRelative(3);
    movePlanar(itodArray(path.sequenceOfPositions[0]) * gridDistance + positionOffset);
    moveVerticalRelative(-3);
    for (auto &position: path.sequenceOfPositions) {
        extrude(itodArray(position) * gridDistance + positionOffset);
    }
}





