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

void Hyrel::defineToolOffset(unsigned int toolNumber, const std::vector<double> &xy, unsigned int offsetRegister) {
    const double xMax = 200;
    const double yMax = 200;
    if (0 > xy[0] || xy[0] > xMax) {
        std::cout << "Gcode writing -> defineToolOffset -> tool x coordinate: max value "
                  << xMax << ", used value " << xy[0] << "\n";
    } else if (0 > xy[1] || xy[1] > yMax) {
        std::cout << "Gcode writing -> defineToolOffset -> tool x coordinate: max value "
                  << yMax << ", used value " << xy[1] << "\n";
    } else {
        generalCommand(
                {'M', 'T', 'O', 'X', 'Y'},
                {true, true, true, false, false},
                {6, (double) toolNumberVariable(toolNumber), (double) offsetRegister, xy[0], xy[1]});
//        bodyStream << "M6 " << toolNumberString(toolNumber) << " X" << xy[0] << " Y" << xy[1] << "\n";
    }
}

void Hyrel::extrudeHyrel(const std::valarray<double> &xy) {
    positions[0] = xy[0];
    positions[1] = xy[1];
//    bodyStream << "G1 X" << xy[0] << " Y" << xy[1] << " F" << printSpeed << " E1" << "\n";
    generalCommand({'G', 'X', 'Y', 'F', 'E'}, {true, false, false, true, true},
                   {0, xy[0], xy[1], (double) moveSpeed, 1});
}

void Hyrel::initHyrel(int hotendTemperature, int bedTemperature, double cleanLength, double nozzleWidth,
                      double layerHeight, int toolNumber, double zOffset, double xOffset, double yOffset) {
    autoHome();
    defineHeightOffset(1, zOffset);
    defineToolOffset(toolNumber, {xOffset, yOffset}, 0);

    selectTool(toolNumber);
    setTemperatureHotend(bedTemperature);
    setTemperatureBed(bedTemperature);


}

