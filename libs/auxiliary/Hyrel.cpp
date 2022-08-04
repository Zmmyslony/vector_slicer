//
// Created by Michał Zmyślony on 04/08/2022.
//

#include <iostream>
#include "Hyrel.h"


int toolNumberVariable(unsigned int toolNumber) {
    const unsigned int numberOfTools = 4;
    if (toolNumber > numberOfTools) {
        std::cout << "Gcode writing -> tool number: max value: "
                  << numberOfTools - 1 << ", used value: " << toolNumber << "\n";
        return -1;
    } else {
        return toolNumber;
    }
}


std::string toolNumberString(unsigned int toolNumber) {
    const unsigned int numberOfTools = 4;
    if (toolNumber > numberOfTools) {
        std::cout << "Gcode writing -> tool number: max value: "
                  << numberOfTools - 1 << ", used value: " << toolNumber << "\n";
        return "";
    } else {
        return "T" + std::to_string(toolNumberVariable(toolNumber));
    }
}

void Hyrel::selectTool(unsigned int toolNumber) {
    const unsigned int numberOfTools = 4;
    if (toolNumber < numberOfTools) {
        generalCommand('T', (int) toolNumber);
    }
}

void Hyrel::defineHeightOffset(unsigned int registerNumber, double height) {
// TODO Obtain max tool numbers from a new class of Printer -> Hyrel and automate retrieving maximal values
    const double maxHeight = 120;
    if (0 > height || height > maxHeight) {
        std::cout << "Gcode writing -> defineHeightOffset -> tool height: max value "
                  << maxHeight << ", used value " << height << "\n";
    } else {
        bodyStream << "M660 H" << registerNumber << " Z" << height << "\n";
    }
}

void Hyrel::defineToolOffset(unsigned int toolNumber, const std::vector<double> &xyz, unsigned int offsetRegister) {
    const double X_MAX = 200;
    const double Y_MAX = 200;
    const double Z_MAX = 120;
    if (0 > xyz[0] || xyz[0] > X_MAX) {
        std::cout << "Gcode writing -> defineToolOffset -> tool x coordinate: max value "
                  << X_MAX << ", used value " << xyz[0] << "\n";
    } else if (0 > xyz[1] || xyz[1] > Y_MAX) {
        std::cout << "Gcode writing -> defineToolOffset -> tool y coordinate: max value "
                  << Y_MAX << ", used value " << xyz[1] << "\n";
    } else if (0 > xyz[2] || xyz[2] > Z_MAX) {
        std::cout << "Gcode writing -> defineToolOffset -> tool z coordinate: max value "
                  << Z_MAX << ", used value " << xyz[2] << "\n";
    } else {
        generalCommand(
                {'M', 'T', 'O', 'X', 'Y', 'Z'},
                {true, true, true, false, false},
                {6, (double) toolNumberVariable(toolNumber), (double) offsetRegister, xyz[0], xyz[1], xyz[2]});
    }
}

void Hyrel::extrudeHyrel(const std::valarray<double> &xy) {
    positions[0] = xy[0];
    positions[1] = xy[1];
//    bodyStream << "G1 X" << xy[0] << " Y" << xy[1] << " F" << printSpeed << " E1" << "\n";
    generalCommand({'G', 'X', 'Y', 'F', 'E'}, {true, false, false, true, true},
                   {0, xy[0], xy[1], (double)moveSpeed, 1});
}

void Hyrel::configureFlow(double nozzleWidth, double layerHeight, double flowMultiplier, int pulses, int tool) {
    generalCommand({'G', 'W', 'Z', 'S', 'P', 'T'}, {true, false, false, false, true, true},
                   {221, nozzleWidth, layerHeight, flowMultiplier, (double) pulses, (double) tool});
}

void Hyrel::setUnitsToMillimetres() {
    generalCommand('G', 21);
}

void Hyrel::turnMotorsOff() {
    generalCommand('M', 18);
}

void Hyrel::signalFinshedPrint() {
    generalCommand('M', 30);
}

void Hyrel::clearOffsets() {
    generalCommand('G', 53);
}

void Hyrel::init(int hotendTemperature, int bedTemperature, double cleanLength, double nozzleWidth,
                 double layerHeight, int toolNumber, double zOffset, double xOffset, double yOffset,
                 double flowMultiplier) {
    const int HEIGHT_OFFSET_REGISTER = 1;
    const int POSITION_OFFSET_REGISTER = 0;
    const int KRA2_PULSES_PER_MICROLITRE = 1297;

    setUnitsToMillimetres();
    setAbsolutePositioning();
    clearOffsets();

    autoHome();
    defineToolOffset(toolNumber, {xOffset, yOffset, zOffset}, POSITION_OFFSET_REGISTER);

    selectTool(toolNumber);
    setTemperatureHotend(bedTemperature);
    setTemperatureBed(bedTemperature);

    configureFlow(nozzleWidth, layerHeight, flowMultiplier, KRA2_PULSES_PER_MICROLITRE, toolNumber);
}

void Hyrel::shutDown() {
    setTemperatureBed(0);
    setTemperatureHotend(0);

    autoHome();
    turnMotorsOff();
    signalFinshedPrint();
}